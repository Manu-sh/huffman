#!/bin/bash

#time ./huffman < ../data/divina_commedia.txt > divina_commedia.txt.hafe

OUTS=()
for  i in ../data/*; do
	name=$(basename "$i")
	out="${name}.hafe"
	echo "compressing $i $out"
	time ./huffman < "$i" > "$out"
	echo ''
	du -h "$i" "$out"
	echo -e '\n-----------\n'
	OUTS+=( $out )
done

for i in ${OUTS[@]}; do
	name="${i%%.hafe}" # strip away .hafe suffix
	echo "decompressing $i -> $name"
	time ./huffman - < "${i}" > "$name"
	md5sum "$name" "../data/$name"
	echo -e '\n-----------\n'
done

