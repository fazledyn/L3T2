#!/usr/bin/bash

#   CSE 314 - Operating System Sessional
#   Offline 1 - Shell Scripting
#   submitted by 1705066 (github.com/fazledyn)

declare USAGE="usage: script.sh [INPUT FILE] [WORKING DIR]"
declare OUTPUT_FILE="output.csv"
declare OUTPUT_DIR="output"

declare INPUT_FILE
declare WORKING_DIR
declare EXCLUDE_FILES

declare -A CSV_TABLE

#   ##############################################
#   Helper methods to make life easier

create_csv() {
    echo "file_type,file_count" >> $OUTPUT_FILE
    for key in ${!CSV_TABLE[@]}
    do
        echo "$key,${CSV_TABLE[$key]}" >> $OUTPUT_FILE
    done
    echo "ignored,$ignored_count" >> $OUTPUT_FILE
}

item_is_ignored() {
    # usage: item_in_array [word]
    # return 1 : if item is ignored, and not to be indexed
    #        0 : if item isn't ignored and needs to be indexed

    WORD=$1
    RETURN_VAL=0
    ARRAY_SIZE="${#EXCLUDE_FILES[@]}"
    
    for (( i=0; i < $ARRAY_SIZE; i++ ))
    do
        if [[ *"${EXCLUDE_FILES[$i]}"* == *"$WORD"* ]]
        then
            RETURN_VAL=1
            break
        fi
    done
    echo $RETURN_VAL
}

#   ##############################################
#   Cleans previous output files and directories
#   if already exists.

if [ -d $OUTPUT_DIR ]
then
    rm -r $OUTPUT_DIR
fi

if [ -f $OUTPUT_FILE ]
then
    rm $OUTPUT_FILE
fi   

#   ##############################################
#   Checks input from the terminal. If not in order
#   prints usage message and others

if ! [ $1 ] && ! [ $2 ]
then
    echo $USAGE
    exit
fi

if [ -f $1 ]
then
    INPUT_FILE=$1
else
    echo "Provided input file doesn't exist"
    echo -n "Enter input file: "
    read INPUT_FILE
fi

if [ -d $2 ]
then
    WORKING_DIR=$2
else
    echo "Working directory not provided"
    echo "Using current directory as root"
    WORKING_DIR=./
fi

#   ##############################################
#   Starts reading from the input file line by line
#   and stores it in array called `EXCLUDE_FILES`

index=0
while IFS= read line || [ -n "$line" ]
do
    EXCLUDE_FILES[index]=$line
    (( index = index + 1 ))
done < $INPUT_FILE


#   ############################################
#   We use `find` command to iterate over directories
#   and only check for files, so we use `f` flag

ignored_count=0
for each in `find $WORKING_DIR -type f`
do
    file_name=${each##*/}
    file_ext=${file_name##*.}

    if [ $file_name = $file_ext ]
    then
        new_dir="$OUTPUT_DIR/others"
        mkdir -p $new_dir
        cp $each $new_dir

        new_doc="$OUTPUT_DIR/others/_list_.txt"
        echo $each >> $new_doc

        (( CSV_TABLE["others"] = "${CSV_TABLE["others"]}" + 1 ))

    else
        if [ $(item_is_ignored $file_ext) -eq 0 ]
        then
            new_dir="$OUTPUT_DIR/$file_ext"
            mkdir -p $new_dir
            if [ -f "$OUTPUT_DIR/$file_ext/$file_name" ]
            then
                parent=$(dirname "$each")
                new_dir="$new_dir/duplicate/$parent/"
                mkdir -p $new_dir
                cp $each $new_dir
            else
                cp $each $new_dir
            fi

            new_doc="$OUTPUT_DIR/$file_ext/_list_.txt"
            echo $each >> $new_doc

            (( CSV_TABLE[$file_ext] = "${CSV_TABLE[$file_ext]}" + 1 ))
        else
            (( ignored_count = $ignored_count + 1 ))
        fi
    fi
done

create_csv
echo "Done!"