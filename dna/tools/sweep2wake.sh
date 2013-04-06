#!/sbin/sh


#get sweep2wake setting
val=$(cat /tmp/aroma-data/sweep.prop | cut -d"=" -f2)
case $val in
  1)
    #disabled
    s2w="0"
    sweep2wakeoff
    ;;
  2)
    #on but no button backlight
    s2w="1"
    sweep2wakeon
    ;;
  3)
    #on with backlight
    s2w="2"
    sweep2wakeon
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


