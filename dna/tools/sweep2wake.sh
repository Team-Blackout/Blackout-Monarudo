#!/sbin/sh


sweep2wakeon()
{
	#Delete the line containing the HTC screenshot feature out of the build.prop
	#regardless of the setting and replace it with the disable command.
	#This is a compatibility fix for sweep2wake (avoids taking screenshots while locking the phone)
	if [ -e /system/build.prop ] ; then
		found=$(find /system/build.prop -type f | xargs grep -oh "ro.htc.framework.screencapture = false");
		if [ "$found" != 'ro.htc.framework.screencapture = false' ]; then
			#delete that line completely regardless of the setting
			sed '/ro.htc.framework.screencapture/d' /system/build.prop > /tmp/tmpfile
			rm /system/build.prop
			mv /tmp/tmpfile /system/build.prop
			#append the new lines for this option at the bottom
			found=$(find /system/build.prop -type f | xargs grep -oh "ro.htc.framework.screencapture");
			if [ "$found" != 'ro.htc.framework.screencapture' ]; then
				echo "\n" >> /system/build.prop;
				echo "ro.htc.framework.screencapture = false" >> /system/build.prop;
				echo "\n" >> /system/build.prop;
			fi
		fi
	fi
}

sweep2wakeoff()
{
	#Delete the line containing the HTC screenshot feature out of the build.prop
	#regardless of the setting and replace it with the enable command.
	#This is reverts the compatibility fix for sweep2wake
	if [ -e /system/build.prop ] ; then
		found=$(find /system/build.prop -type f | xargs grep -oh "ro.htc.framework.screencapture = true");
		if [ "$found" != 'ro.htc.framework.screencapture = true' ]; then
			#delete that line completely regardless of the setting
			sed '/ro.htc.framework.screencapture/d' /system/build.prop > /tmp/tmpfile
			rm /system/build.prop
			mv /tmp/tmpfile /system/build.prop
			#append the new lines for this option at the bottom
			found=$(find /system/build.prop -type f | xargs grep -oh "ro.htc.framework.screencapture");
			if [ "$found" != 'ro.htc.framework.screencapture' ]; then
				echo "\n" >> /system/build.prop;
				echo "ro.htc.framework.screencapture = true" >> /system/build.prop;
				echo "\n" >> /system/build.prop;
			fi
		fi
	fi
}

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
val=$(cat /tmp/aroma-data/sweep_start.prop | cut -d"=" -f2)
case $val in
  1)
    s2w_start="BACK"
    ;;
  2)
    s2w_start="HOME"
    ;;
  3)
    s2w_start="MENU"
    ;;
esac

val=$(cat /tmp/aroma-data/sweep_end.prop | cut -d"=" -f2)
case $val in
  1)
    s2w_end="HOME"
    ;;
  2)
    s2w_end="MENU"
    ;;
esac
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

cmdline=$(cat /tmp/boot.img-cmdline)
searchString="s2w_start="
s2w_start="s2w_start="$s2w_start
case $cmdline in
  "$searchString"* | *" $searchString"*)
   	echo $(cat /tmp/boot.img-cmdline | sed -e 's/s2w_start=[^ ]\+//')>/tmp/boot.img-cmdline
	echo $(cat /tmp/boot.img-cmdline)\ $s2w_start>/tmp/boot.img-cmdline
	;;  
  *)
	echo $(cat /tmp/boot.img-cmdline)\ $s2w_start>/tmp/boot.img-cmdline
	;;
esac

cmdline=$(cat /tmp/boot.img-cmdline)
searchString="s2w_end="
s2w_end="s2w_end="$s2w_end
case $cmdline in
  "$searchString"* | *" $searchString"*)
   	echo $(cat /tmp/boot.img-cmdline | sed -e 's/s2w_end=[^ ]\+//')>/tmp/boot.img-cmdline
	echo $(cat /tmp/boot.img-cmdline)\ $s2w_end>/tmp/boot.img-cmdline
	;;  
  *)
	echo $(cat /tmp/boot.img-cmdline)\ $s2w_end>/tmp/boot.img-cmdline
	;;
esac
