#!/bin/bash

function show_help()
{
    echo -e "Usage: ./scipt.sh [--help|-h] [--lex] [--syn] [--sem] [--exe]\n"
    echo "optional arguments:"
    echo -e "-h, --help\t show this help message and exit"
    echo -e "--lex\t\t testing lexer"
    echo -e "--syn\t\t testing syntax"
    echo -e "--sem\t\t testing semantics"
    echo -e "--exe\t\t testing output ifjcode20"
}

lex=0
syn=0
sem=0
exe=0

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --help|-h) show_help; exit 0 ;;
        --lex) lex=1 ;;
        --syn) syn=1 ;;
        --sem) sem=1 ;;
        --exe) exe=1 ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

if [ "$lex" = "0" ] && [ "$syn" = "0" ] && [ "$sem" = 0 ] && [ "$exe" = 0 ]; then
    lex=1; syn=1; sem=1; exe=1
fi

NORMAL=$(tput sgr0)
GREEN=$(tput setaf 2)
RED=$(tput setaf 1)
COUNT_ALL=0
COUNT_OK=0
COUNT_FALSE=0

if [ "$lex" = "1" ]
then 
    for n in $(ls -p -R tests/scanner | grep -E -i '^.+\.go$')
    do
        ((COUNT_ALL+=1))
        name=${n::-3}

        # scanner
        ( bin/ifj20 <tests/scanner/"${name}".go ) >/dev/null
        RC=$?
        if [ "${RC}" = "$(cat tests/scanner/"${name}".rc)" ]
        then
            echo -e "[${GREEN} OK ${NORMAL}]\t\t${name}"
            ((COUNT_OK+=1))
        else
            echo -e "[${RED} FAILED ${NORMAL}]\t${name}\t\tRC=${RC} expected=$(cat tests/scanner/"${name}".rc)"
            ((COUNT_FALSE+=1))
        fi
    done
fi

if [ "$syn" = "1" ]
then
    for n in $(ls -p -R tests/parser | grep -E -i '^.+\.go$')
    do
        ((COUNT_ALL+=1))
        name=${n::-3}

        ( bin/ifj20 <tests/parser/"${name}".go ) >/dev/null
        RC=$?
        if [ "${RC}" = "$(cat tests/parser/"${name}".rc)" ]
        then
            echo -e "[${GREEN} OK ${NORMAL}]\t\t${name}"
            ((COUNT_OK+=1))
        else
            echo -e "[${RED} FAILED ${NORMAL}]\t${name}\t\tRC=${RC} expected=$(cat tests/parser/"${name}".rc)"
            ((COUNT_FALSE+=1))
        fi
    done
fi

if [ "$sem" = "1" ]
then
    for n in $(ls -p -R tests/sem | grep -E -i '^.+\.go$')
    do
        ((COUNT_ALL+=1))
        name=${n::-3}

        ( bin/ifj20 <tests/sem/"${name}".go ) >/dev/null
        RC=$?
        if [ "${RC}" = "$(cat tests/sem/"${name}".rc)" ]
        then
            echo -e "[${GREEN} OK ${NORMAL}]\t\t${name}"
            ((COUNT_OK+=1))
        else
            echo -e "[${RED} FAILED ${NORMAL}]\t${name}\t\tRC=${RC} expected=$(cat tests/sem/"${name}".rc)"
            ((COUNT_FALSE+=1))
        fi
    done
fi

if [ "$exe" = "1" ]
then
    for n in $(ls -p -R tests/exe | grep -E -i '^.+\.go$')
    do
        ((COUNT_ALL+=1))
        name=${n::-3}

        ( bin/ifj20 <tests/exe/"${name}".go ) >/dev/null
        RC=$?
        if [ "${RC}" = "$(cat tests/exe/"${name}".rc)" ]
        then
            echo -e "[${GREEN} OK ${NORMAL}]\t\t${name}"
            ((COUNT_OK+=1))
        else
            echo -e "[${RED} FAILED ${NORMAL}]\t${name}\t\tRC=${RC} expected=$(cat tests/exe/"${name}".rc)"
            ((COUNT_FALSE+=1))
        fi
    done
fi

echo -e "\nTest count: "$COUNT_ALL
echo "${GREEN}Tests passed:${NORMAL}" $COUNT_OK
echo "${RED}Tests failed:${NORMAL}" $COUNT_FALSE