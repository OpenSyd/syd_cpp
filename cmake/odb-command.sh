#!/bin/bash

odb_exec=$1               # ${ODB_EXECUTABLE}
dcmtk_include_dir_base=$2 # ${DCMTK_INCLUDE_DIR_BASE}
syd_source_dir=$3         # ${SYD_SOURCE_DIR}
schema_name=$4            # ${SCHEMA_NAME}
output=$5                 # build/lib
odb_files_abs=$6          # ${ODB_FILES_ABS}

xml="${odb_files_abs%%.*}".xml

echo $1
echo $2
echo $3
echo $4
echo $5
echo $6
echo ${output}
echo "ici"

mkdir -p ${output}


eval  ${odb_exec} --std c++11 --database sqlite -I${dcmtk_include_dir_base} -I${syd_source_dir}/src/std_db -I${syd_source_dir}/src/core  -I${syd_source_dir}/src/common_db  --generate-schema --schema-format embedded  --generate-query --sqlite-override-null --schema-name ${schema_name} ${odb_files_abs}

if [ -f $xml ]; then
   cp ${xml} ${output}/
fi

# b=$(basename $odb_files_abs)
# variable_name=xml_${schema_name}_${b%%.*}
# echo $variable_name
# echo "const char* ${variable_name} = \""   >> $output
# echo `cat ${xml}`  >> $output
# echo "\" ;"                    >> $output
