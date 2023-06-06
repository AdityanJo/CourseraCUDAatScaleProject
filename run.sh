#/app/bin/x86_64/linux/release/imageBlendingNPP -input1=/app/data/samples/1.1.01.tiff -input2=/app/data/samples/1.5.05.tiff -output=/tmp/output_a1_50_a2_250.pgm -alpha1=50 -alpha2=250

# Generate for entire dataset
cd /app/data/
wget https://sipi.usc.edu/database/textures.tar.gz 
tar -zxvf textures.tar.gz 

mkdir /tmp/output
counter=0
for l_f in /app/data/textures/1.1.*.tiff; do
  for r_f in $(ls -1 /app/data/textures/1.2.*.tiff | sort -r)
  do  
   echo $l_f $r_f
   /app/bin/x86_64/linux/release/imageBlendingNPP -input1=$l_f -input2=$r_f -output=/tmp/output/output_$counter.pgm -alpha1=50 -alpha2=250
   counter=$((counter+1))
  done
done

