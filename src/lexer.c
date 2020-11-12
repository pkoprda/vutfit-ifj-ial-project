#include "libmine.h"

Stack stack;

char *displayToken[] = {
    "identifier", ":=", "=",
    "INT", "FLOAT", "STR",
    "+", "-", "*", "/", "<", "<=", ">", ">=", "==", "!=", "EOL",
    ",", ";", "(", ")", "{", "}",
    "package", "if", "else", "for", "func", "int", "float64", "string", "return",
    "main", "inputs", "inputi", "inputf", "print", "int2float", "float2int", "len", "substr", "ord", "chr"};

int lexer()
{
    char c;

    /* Neskor extern aby bol global */
    initStack(&stack);
    int stav = STATE_START;
    string bufferHelp;
    string *buffer = &bufferHelp;

    while (!feof(stdin))
    {

        c = getc(stdin);

        switch (stav)
        {

        // pociatocny stav automatu
        case STATE_START:

            // \r iba pre subory s CRLF
            if (c == '\r' || c == EOF)
                ;

            // novy riadok
            else if (c == '\n')
            {
                debug_print("EOL");
                stackPush(&stack, initToken(TOKEN_EOL, NULL));
            }

            // medzera
            else if (c == '\t' || c == ' ')
            {
                ;
                //debug_print("WHITESPACE");
            }

            // delenie alebo komentar
            else if (c == '/')
            {
                c = getc(stdin);
                if (c == '/' || c == '*')
                {
                    ungetc(c, stdin);
                    stav = STATE_COMMENTS;
                }
                else
                {
                    ungetc(c, stdin);
                    stackPush(&stack, initToken(TOKEN_DIV, NULL));
                }
            }

            // identifikator alebo klucove slovo
            else if (isalpha(c) || c == '_')
            {
                initString(buffer);
                buffer = initBuffer();
                addChar(buffer, c);
                stav = STATE_IDENTIF;
            }

            // cislo - int alebo float64
            else if (isdigit(c))
            {
                ungetc(c, stdin);

                initString(buffer);
                buffer = initBuffer();
                stav = STATE_NUMBER;
            }

            // zaciatok stringu
            else if (c == '"')
            {
                ungetc(c, stdin);
                initString(buffer);
                buffer = initBuffer();
                addChar(buffer, c);
                stav = STATE_STRING;
            }

            // definicia premennej
            else if (c == ':')
            {
                c = getc(stdin);
                if (c == '=')
                {
                    stackPush(&stack, initToken(IDENTIF_DEF, NULL));
                }
                else
                {
                    error_exit(LEX_ERROR, "Lexikalna chyba!");
                }
            }

            // operatory
            else if (c == '+')
                stackPush(&stack, initToken(TOKEN_PLUS, NULL));
            else if (c == '-')
                stackPush(&stack, initToken(TOKEN_MINUS, NULL));
            else if (c == '*')
                stackPush(&stack, initToken(TOKEN_MUL, NULL));
            else if (c == '<')
            {
                c = getc(stdin);
                if (c == '=')
                {
                    stackPush(&stack, initToken(TOKEN_LESS_EQUAL, NULL));
                }
                else
                {
                    ungetc(c, stdin);
                    stackPush(&stack, initToken(TOKEN_LESS, NULL));
                }
            }
            else if (c == '>')
            {
                c = getc(stdin);
                if (c == '=')
                {
                    stackPush(&stack, initToken(TOKEN_GREATER_EQUAL, NULL));
                }
                else
                {
                    ungetc(c, stdin);
                    stackPush(&stack, initToken(TOKEN_GREATER, NULL));
                }
            }
            else if (c == '=')
            {
                c = getc(stdin);
                if (c == '=')
                {
                    stackPush(&stack, initToken(TOKEN_EQUAL, NULL));
                }
                else
                {
                    // inicializacia premennej
                    ungetc(c, stdin);
                    stackPush(&stack, initToken(IDENTIF_INIT, NULL));
                }
            }
            else if (c == '!')
            {
                c = getc(stdin);
                if (c == '=')
                {
                    stackPush(&stack, initToken(TOKEN_NOT_EQUAL, NULL));
                }
                else
                {
                    error_exit(LEX_ERROR, "Lexikalna chyba!");
                }
            }

            // oddelovace
            else if (c == ',')
                stackPush(&stack, initToken(TOKEN_COMMA, NULL));
            else if (c == ';')
                stackPush(&stack, initToken(TOKEN_SEMICOLON, NULL));
            else if (c == '(')
                stackPush(&stack, initToken(TOKEN_ROUND_LBRACKET, NULL));
            else if (c == ')')
                stackPush(&stack, initToken(TOKEN_ROUND_RBRACKET, NULL));
            else if (c == '{')
                stackPush(&stack, initToken(TOKEN_CURLY_LBRACKET, NULL));
            else if (c == '}')
                stackPush(&stack, initToken(TOKEN_CURLY_RBRACKET, NULL));

            // nezname znaky
            else
            {
                debug_print("\n%c %d", c, c);
                error_exit(LEX_ERROR, "Lexikalna chyba!");
            }

            break;

        // riadkove a blokove komentare
        case STATE_COMMENTS:

            if (c == '/')
            {
                // Jednoriadkovy komentar
                c = getc(stdin);
                while (c != '\n')
                {
                    if(c == EOF){
                        break;
                    }
                    c = getc(stdin);
                }
                stackPush(&stack, initToken(TOKEN_EOL, NULL));
                stav = STATE_START;
            }
            else if (c == '*')
            {
                // Blokovy komentar
                char nextChar;
                while ((c = getc(stdin)))
                {
                    nextChar = getc(stdin);
                    if (c == '*' && nextChar == '/')
                    {
                        stav = STATE_START;
                        break;
                    }
                    ungetc(nextChar, stdin);

                    if (c == '\n')
                    {
                        stackPush(&stack, initToken(TOKEN_EOL, NULL));
                    }

                    if (feof(stdin))
                    {
                        error_exit(LEX_ERROR, "Lexikalna chyba!");
                    }
                }
            }
            break;

        case STATE_IDENTIF:

            while (1)
            {
                if (isalpha(c) || isdigit(c) || c == '_')
                {
                    addChar(buffer, c);
                    c = getc(stdin);
                }
                else
                {
                    ungetc(c, stdin);
                    break;
                }
            }

            // keywordy a funkcie
            if (MIN_LEN_KEYWORD <= strlen(buffer->str) && strlen(buffer->str) <= MAX_LEN_KEYWORD)
            {
                if (!strcmp(buffer->str, "else"))
                    stackPush(&stack, initToken(KEYWORD_ELSE, NULL));
                else if (!strcmp(buffer->str, "float64"))
                    stackPush(&stack, initToken(KEYWORD_FLOAT64, NULL));
                else if (!strcmp(buffer->str, "for"))
                    stackPush(&stack, initToken(KEYWORD_FOR, NULL));
                else if (!strcmp(buffer->str, "func"))
                    stackPush(&stack, initToken(KEYWORD_FUNC, NULL));
                else if (!strcmp(buffer->str, "if"))
                    stackPush(&stack, initToken(KEYWORD_IF, NULL));
                else if (!strcmp(buffer->str, "int"))
                    stackPush(&stack, initToken(KEYWORD_INT, NULL));
                else if (!strcmp(buffer->str, "package"))
                    stackPush(&stack, initToken(KEYWORD_PACKAGE, NULL));
                else if (!strcmp(buffer->str, "return"))
                    stackPush(&stack, initToken(KEYWORD_RETURN, NULL));
                else if (!strcmp(buffer->str, "string"))
                    stackPush(&stack, initToken(KEYWORD_STRING, NULL));
                else if (!strcmp(buffer->str, "main"))
                    stackPush(&stack, initToken(FUNC_MAIN, "main"));
                else if (!strcmp(buffer->str, "inputs"))
                    stackPush(&stack, initToken(FUNC_INPUTS, NULL));
                else if (!strcmp(buffer->str, "inputi"))
                    stackPush(&stack, initToken(FUNC_INPUTI, NULL));
                else if (!strcmp(buffer->str, "inputf"))
                    stackPush(&stack, initToken(FUNC_INPUTF, NULL));
                else if (!strcmp(buffer->str, "print"))
                    stackPush(&stack, initToken(FUNC_PRINT, NULL));
                else if (!strcmp(buffer->str, "int2float"))
                    stackPush(&stack, initToken(FUNC_INT2FLOAT, NULL));
                else if (!strcmp(buffer->str, "float2int"))
                    stackPush(&stack, initToken(FUNC_FLOAT2INT, NULL));
                else if (!strcmp(buffer->str, "len"))
                    stackPush(&stack, initToken(FUNC_LEN, NULL));
                else if (!strcmp(buffer->str, "substr"))
                    stackPush(&stack, initToken(FUNC_SUBSTR, NULL));
                else if (!strcmp(buffer->str, "ord"))
                    stackPush(&stack, initToken(FUNC_ORD, NULL));
                else if (!strcmp(buffer->str, "chr"))
                    stackPush(&stack, initToken(FUNC_CHR, NULL));
                else
                {
                    // identifikator
                    stackPush(&stack, initToken(TOKEN_IDENTIF, my_strdup(buffer->str)));
                    debug_print("IDENTIFIER: %s", buffer->str);
                    clearBuffer(buffer);
                    freeBuffer(buffer);
                    buffer = &bufferHelp;
                    stav = STATE_START;
                    break;
                }
                debug_print("KEYWORD: %s", buffer->str);
            }

            // identifikator
            else
            {
                stackPush(&stack, initToken(TOKEN_IDENTIF, my_strdup(buffer->str)));
                debug_print("IDENTIFIER: %s", buffer->str);
            }
            clearBuffer(buffer);
            freeBuffer(buffer);
            buffer = &bufferHelp;
            stav = STATE_START;
            break;

        case STATE_NUMBER:

            if (isdigit(c))
            {
                addChar(buffer, c);
                stav = STATE_NUMBER;
            }
            else if (c == '.')
            {
                addChar(buffer, c);
                c = getc(stdin);
                if (isdigit(c))
                {
                    addChar(buffer, c);
                    stav = STATE_FLOAT_NUMBER;
                }
                else
                {
                    error_exit(LEX_ERROR, "Lexikalna chyba!");
                }
            }
            else if (c == 'e' || c == 'E')
            {
                stav = STATE_EXPONENT_NUMBER;
            }
            else
            {
                ungetc(c, stdin);
                stackPush(&stack, initToken(TOKEN_INT, my_strdup(buffer->str)));
                clearBuffer(buffer);
                freeBuffer(buffer);
                buffer = &bufferHelp;
                stav = STATE_START;
            }

            break;

        case STATE_FLOAT_NUMBER:

            addChar(buffer, c);
            if (isdigit(c))
            {
                stav = STATE_FLOAT_NUMBER;
            }
            else if (c == 'e' || c == 'E')
            {
                stav = STATE_EXPONENT_NUMBER;
            }
            else
            {
                ungetc(c, stdin);
                stackPush(&stack, initToken(TOKEN_FLOAT, my_strdup(buffer->str)));
                clearBuffer(buffer);
                freeBuffer(buffer);
                buffer = &bufferHelp;
                stav = STATE_START;
            }
            break;

        case STATE_EXPONENT_NUMBER:
            if (c == '+' || c == '-')
            {
                addChar(buffer, c);
                c = getc(stdin);
                if (isdigit(c))
                {
                    addChar(buffer, c);
                    stav = STATE_EXPONENT_NUMBER_FINAL;
                }
                else
                {
                    error_exit(LEX_ERROR, "Lexikalna chyba!");
                }
            }
            else if (isdigit(c))
            {
                addChar(buffer, c);
                stav = STATE_EXPONENT_NUMBER_FINAL;
            }
            else
            {
                clearBuffer(buffer);
                freeBuffer(buffer);
                error_exit(LEX_ERROR, "Lexikalna chyba!");
            }
            break;

        case STATE_EXPONENT_NUMBER_FINAL:
            addChar(buffer, c);
            if (isdigit(c))
            {
                stav = STATE_EXPONENT_NUMBER_FINAL;
            }
            else
            {
                ungetc(c, stdin);
                stackPush(&stack, initToken(TOKEN_FLOAT, buffer->str));
                clearBuffer(buffer);
                freeBuffer(buffer);
                buffer = &bufferHelp;
                stav = STATE_START;
            }
            break;

        case STATE_STRING:
            while (1)
            {
                c = getc(stdin);

                // koniec stringu
                if (c == '"')
                {
                    addChar(buffer, c);
                    debug_print("STRING: %s", buffer->str);
                    stackPush(&stack, initToken(TOKEN_STRING, my_strdup(buffer->str)));
                    clearBuffer(buffer);
                    freeBuffer(buffer);
                    buffer = &bufferHelp;
                    stav = STATE_START;
                    break;
                }

                // ASCII hodnota mensia ako 32
                else if (c < 32)
                {
                    clearBuffer(buffer);
                    freeBuffer(buffer);
                    error_exit(LEX_ERROR, "Lexikalna chyba!");
                }

                // escape sekvencie
                else if (c == '\\')
                {
                    addChar(buffer, c);
                    c = getc(stdin);

                    //  '\"'  '\n'  '\t'  '\\'
                    if (c == '"' || c == 'n' || c == 't' || c == '\\')
                    {
                        addChar(buffer, c);
                        continue;
                    }

                    // hexadecimalne escape sekvencia \xhh, kde hh je cislo od 00 do FF
                    else if (c == 'x')
                    {
                        addChar(buffer, c);
                        c = getc(stdin);
                        if (isxdigit(c))
                        {
                            addChar(buffer, c);
                            c = getc(stdin);
                            if (isxdigit(c))
                            {
                                addChar(buffer, c);
                                continue;
                            }
                            else
                            {
                                clearBuffer(buffer);
                                freeBuffer(buffer);
                                error_exit(LEX_ERROR, "Lexikalna chyba!");
                            }
                        }
                        else
                        {
                            clearBuffer(buffer);
                            freeBuffer(buffer);
                            error_exit(LEX_ERROR, "Lexikalna chyba!");
                        }
                    }
                    else
                    {
                        clearBuffer(buffer);
                        freeBuffer(buffer);
                        error_exit(LEX_ERROR, "Lexikalna chyba!");
                    }
                }

                else
                {
                    addChar(buffer, c);
                }
            }

            break;

        default:
            error_exit(LEX_ERROR, "Lexikalna chyba!");
        }
    }

    stackFlip(&stack);
    stackPrint(&stack, displayToken);
    return SCANNER_OK;
}