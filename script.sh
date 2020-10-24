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

    if [ -f "tests/"${name}".go" ]
    then
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
    else
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
    fi
    
done

echo -e "\nTest count: "$COUNT_ALL
echo "${GREEN}Tests passed:${NORMAL}" $COUNT_OK
echo "${RED}Tests failed:${NORMAL}" $COUNT_FALSE