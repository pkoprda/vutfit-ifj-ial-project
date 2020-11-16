#!/bin/bash

NORMAL=$(tput sgr0)
GREEN=$(tput setaf 2)
RED=$(tput setaf 1)
COUNT_ALL=0
COUNT_OK=0
COUNT_FALSE=0

for n in $(ls -p -R tests/ | grep -E -i '^.+\.go$')
do
    ((COUNT_ALL+=1))
    name=${n::-3}

    # scanner
    if [ -f "tests/scanner/"${name}".go" ]
    then
        ( bin/main <tests/scanner/"${name}".go ) >/dev/null
        RC=$?
        if [ "${RC}" = "$(cat tests/scanner/"${name}".rc)" ]
        then
            echo -e "[${GREEN} OK ${NORMAL}]\t\t${name}"
            ((COUNT_OK+=1))
        else
            echo -e "[${RED} FAILED ${NORMAL}]\t${name}\t\tRC=${RC} expected=$(cat tests/scanner/"${name}".rc)"
            ((COUNT_FALSE+=1))
        fi

    # parser
    elif [ -f "tests/parser/"${name}".go" ]
    then
        ( bin/main <tests/parser/"${name}".go ) >/dev/null
        RC=$?
        if [ "${RC}" = "$(cat tests/parser/"${name}".rc)" ]
        then
            echo -e "[${GREEN} OK ${NORMAL}]\t\t${name}"
            ((COUNT_OK+=1))
        else
            echo -e "[${RED} FAILED ${NORMAL}]\t${name}\t\tRC=${RC} expected=$(cat tests/parser/"${name}".rc)"
            ((COUNT_FALSE+=1))
        fi

    elif [ -f "tests/sem/"${name}".go" ]
    then
        ( bin/main <tests/sem/"${name}".go ) >/dev/null
        RC=$?
        if [ "${RC}" = "$(cat tests/sem/"${name}".rc)" ]
        then
            echo -e "[${GREEN} OK ${NORMAL}]\t\t${name}"
            ((COUNT_OK+=1))
        else
            echo -e "[${RED} FAILED ${NORMAL}]\t${name}\t\tRC=${RC} expected=$(cat tests/sem/"${name}".rc)"
            ((COUNT_FALSE+=1))
        fi

    # anything else
    else
        ( bin/main <tests/"${name}".go ) >/dev/null
        RC=$?
        if [ "${RC}" = "$(cat tests/"${name}".rc)" ]
        then
            echo -e "[${GREEN} OK ${NORMAL}]\t\t${name}"
            ((COUNT_OK+=1))
        else
            echo -e "[${RED} FAILED ${NORMAL}]\t${name}\t\tRC=${RC} expected=$(cat tests/"${name}".rc)"
            ((COUNT_FALSE+=1))
        fi
    fi
done

echo -e "\nTest count: "$COUNT_ALL
echo "${GREEN}Tests passed:${NORMAL}" $COUNT_OK
echo "${RED}Tests failed:${NORMAL}" $COUNT_FALSE