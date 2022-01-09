#p=/mnt/c/tmp/RML/build/Source/morph_dict/TestLem/RelWithDebInfo/TestLem.exe 
p=/mnt/c/tmp/RML/out/build/x64-Release/Source/morph_dict/TestLem/TestLem.exe

for i in `seq 1 10`; do 
    echo $i
    for l in Russian English German; do
        f=C:/tmp/RML/Source/morph_dict/TestLem/test/$l/test.txt 
        $p --language $l --input $f --output $f.tmp
    done
done