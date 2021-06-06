#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TIGER_DIR="$DIR"
TIGER="$TIGER_DIR"/tiger.exe

RED='\033[1;31m'
GREEN='\033[1;32m'
CYAN='\033[1;36m'
NC='\033[0m' # No Color
if [ -z ${DIFFTOOL+x} ]; then
    if hash meld 2>/dev/null; then
        DIFFTOOL="diff"
    else
        DIFFTOOL="diff"
    fi
fi

function OK(){
	echo -e "${GREEN}OK${NC}"
}

function BAD(){
	echo -e "${RED}BAD${NC}"
}

function usage(){
    echo -e "Usage: ${GREEN}$0 run ${NC}Runs all testcases"
    echo -e ""
    echo
    echo -e "Note: file.tig is considered to be a testcase if file.out exists."
    exit 1
}

function build(){
    printf "Compiling Tiger... "
    make -C "$TIGER_DIR" >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        BAD
        make -C "$TIGER_DIR"
        exit 1
    fi
    OK
}

function gen_output(){
    exe="${source/.tig/}"
    rm -f $exe || true
    source=${1%.*}.tig
    cp $source "$DIR/test.file"
    result=`$TIGER | grep error`
    if [ ! -z "$result" ]; then
        echo "$result"
    else
        "$TIGER_DIR/run.sh" 2>&1
        echo Return code: $?
        if [ -f "$exe" ]; then
            $exe 2>&1 </dev/null
            echo Return code: $?
            rm -f $exe
        fi
    fi
}

function print_case(){
    source=${1%.*}.tig
    if [ -f "$source" ]; then
        if [ "$#" -ne 1 ]; then
			output=$2
		else
			output=$(gen_output "$source" 2>&1)
		fi
		correct=${source%.*}.out
		if [ -f "$correct" ]; then
            if [ "$#" -gt 2 ]; then
                if [ "$3" -ne 0 ]; then
                    echo -e "${CYAN}Source code:${NC}"
                    cat "$source"
                    echo -e "${CYAN}Diff:${NC}"
			        $DIFFTOOL "$correct" <(echo "$output")
                    echo
                fi
            fi
        else
            echo -e "${CYAN}Output:${NC}"
            echo "$output"
		fi
    fi
}

function run_case(){
    source=${1%.*}.tig
    if [ -f "$source" ]; then
        correct=${source%.*}.out
        printf "* Testing case "${source#$DIR/}"... "
        if [ -f "$correct" ]; then
			trap "echo Case stopped" INT
			output=$(gen_output "$source")
            if [ ! -z "`echo $output | grep \"Segmentation fault\" | grep \"core dumped\" | grep \"a.out\"`" ]; then
                output="Segmentation fault while running compiled program"
            fi
			trap - INT
            diff <(echo "$output") "$correct" > /dev/null
            if [ $? -eq 0 ]; then
                OK
                if [ "$#" -ne 1 ]; then
                    print_case "$source" "$output"
                fi
                return 0
            else
                BAD
                print_case "$source" "$output" 1
                return 1
            fi
        else
            echo
            print_case "$source"
            return 1
        fi
    else
        echo -e "${RED}File $source not found.${NC}"
        return 2
    fi
}


verb="$1"
shift
case "$verb" in
    run)
        build
        passed=0
        tot=0
        if [ "$#" -eq 0 ] || [ "$1" = "all" ]; then
            for tfile in $(find "$DIR" -name "*.tig" | sort); do
                correct=${tfile%.*}.out
                if [ -f "$correct" ]; then
                    run_case "$tfile"
                    passed=$((passed+($? == 0)))
                    tot=$((tot+1))
                fi
            done
        else
            while [ $# -ne 0 ]
              do
                run_case "$1" force_output
                passed=$((passed+($? == 0)))
                tot=$((tot+1))
                shift
            done
        fi
        echo -e "Tests passed ${CYAN}${passed}/${tot}${NC}"
        ;;
    *)
        usage
esac
