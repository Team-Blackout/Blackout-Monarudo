#!/sbin/sh
#sweep2wake.sh by show-p1984
#Features: 
#activate sweep2wake over the kernels cmdline

#get sweep2wake setting
val=$(cat /tmp/aroma-data/sweep.prop | cut -d"=" -f2)
case $val in
  1)
    #on
    s2w="1"
    ;;
  2)
    #off
    s2w="0"
    ;;
esac


#Add s2w to the kernels cmdline.
cmdline=$(cat /tmp/boot.img-cmdline)
searchString="s2w="
s2w="s2w="$s2w
case $cmdline in
  "$searchString"* | *" $searchString"*)
   	echo $(cat /tmp/boot.img-cmdline | sed -e 's/s2w=[^ ]\+//')>/tmp/boot.img-cmdline
	echo $(cat /tmp/boot.img-cmdline)\ $s2w>/tmp/boot.img-cmdline
	;;  
  *)
	echo $(cat /tmp/boot.img-cmdline)\ $s2w>/tmp/boot.img-cmdline
	;;
esac
#end s2w

