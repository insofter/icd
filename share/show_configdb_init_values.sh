#!/bin/bash

cd ${ICDTCP3_DIR}/icd/share

cat init-default-config.sql \
  | awk -F '==' '/section_id/ { 
    gsub(")","",$2); 
    gsub(";","",$2); 
    gsub("--",", --",$2); 
    gsub("'"'"'","",$2); 
    print $2
  }' \
  | sort \
  | column -t -s ','


