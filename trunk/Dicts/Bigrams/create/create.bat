dir *.xml /b /s >xml.con
sed -r "s/(.*)\.xml/gawk -f c:\/Sokirko\/awk\/xml2txt.awk < \1.xml > \1.txt/" <xml.con >xml2txt.bat
call xml2txt.bat
del xml2txt.bat
del xml.con
del *.xml /s
dir *.txt /b /s >txt.con
Bigrams Russian txt.con --only-words  --output  bigrams.hmm  --hmm-config C:\RusNatCorpus2\default_tagset\hmm.config 
