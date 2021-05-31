#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
TIGER_DIR="$DIR"
TIGER="$TIGER_DIR"/tiger.exe

RED='\033[0;31m'
GREEN='\033[0;32m'
PURPLE='\033[0;35m'
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
    echo -e "Usage: $0 <command> [<args>]"
    echo -e "These are the available commands:"
    echo -e "\t${GREEN}add file1.tig file2.tig ...${NC}\tAdd specific cases to the test suite"
    echo -e "\t${GREEN}add [all]${NC}\t\t\tSearch and add cases not in the test suite"
    echo -e "\t${GREEN}run file1.tig file2.tig ...${NC}\tRun (and check) specific cases"
    echo -e "\t${GREEN}run [all]${NC}\t\t\tRun the test suite (all added cases)"
    echo -e "\t${GREEN}fix${NC}\t\t\tRun and ask to correct each failing case"
    echo -e "\t${GREEN}del file1.tig file2.tig ...${NC}\tRemove specific cases from the test suite"
    echo
    echo -e "Note: file.tig is considered to be in the test suite iif file.out exists."
    exit 1
}

function compile_tiger(){
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
			#~ echo -e "${PURPLE}Expected Output:${NC}"
			#~ cat "$correct"
			#~ echo -e "${PURPLE}Diff <expected >actual:${NC}"
            if [ "$#" -gt 2 ]; then
                if [ "$3" -ne 0 ]; then
                    echo -e "${PURPLE}Source code:${NC}"
                    cat "$source"
                    echo -e "${PURPLE}Diff:${NC}"
			        $DIFFTOOL "$correct" <(echo "$output")
                    echo
                fi
            fi
        else
            echo -e "${PURPLE}Output:${NC}"
            echo "$output"
		fi
    fi
}

function add_case(){
    source=${1%.*}.tig
    if [ -f "$source" ]; then
        echo "Running case "${source#$DIR/}"... "
        if [ "$#" -ne 1 ]; then
			output=$2
		else
			output=$(gen_output "$source" 2>&1)
		fi
        print_case "$source" "$output"
        read -p "Add this output? (y/n) " -n 1 -r
        echo
        if [[ $REPLY =~ ^[YysS]$ ]]
        then
            correct=${source%.*}.out
            echo Adding case "${source#$DIR/}"...
            echo "$output" > "$correct"
            OK
        fi
    else
        echo -e "${RED}File $source not found.${NC}"
    fi
}

function del_case(){
    source=${1%.*}.tig
    correct=${source%.*}.out
    if [ -f "$correct" ]; then
        echo Removing case "${correct#$DIR/}"...
        print_case "$source"
        read -p "Remove this output? (y/n) " -n 1 -r
        echo    # (optional) move to a new line
        if [[ $REPLY =~ ^[YysS]$ ]]
        then
            rm -f "$correct"
        fi
    else
        echo -e "${RED}Output $correct not found.${NC}"
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

function fix_case(){
    source=${1%.*}.tig
    if [ -f "$source" ]; then
        correct=${source%.*}.out
        printf "* Testing case "${source#$DIR/}"... "
        if [ -f "$correct" ]; then
			output=$(gen_output "$source")
            diff <(echo "$output") "$correct" > /dev/null
            if [ $? -eq 0 ]; then
                OK
                return 0
            else
                BAD
                add_case "$source" "$output"
                #~ print_case "$source"
                return 1
            fi
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
        compile_tiger
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
        echo -e "Tests passed ${PURPLE}${passed}/${tot}${NC}"
        ;;
    fix)
        compile_tiger
        if [ "$#" -eq 0 ] || [ "$1" = "all" ]; then
            for tfile in $(find "$DIR" -name "*.tig" | sort); do
                correct=${tfile%.*}.out
                if [ -f "$correct" ]; then
                    fix_case "$tfile"
                fi
            done
        else
            while [ $# -ne 0 ]
              do
                fix_case "$1"
                shift
            done
        fi
        ;;
     
    add)
        compile_tiger
        if [ "$#" -eq 0 ] || [ "$1" = "all" ]; then
            for tfile in $(find "$DIR" -name "*.tig" | sort); do
                correct=${tfile%.*}.out
                if [ ! -f "$correct" ]; then
                    add_case "$tfile"
                fi
            done
        else
            while [ $# -ne 0 ]
              do
                add_case "$1"
                shift
            done
        fi
        ;;
     
    del)
        [ "$#" -eq 0 ] && usage
        while [ $# -ne 0 ]
          do
            del_case "$1"
            shift
        done
        ;;
    *)
        usage
esac
