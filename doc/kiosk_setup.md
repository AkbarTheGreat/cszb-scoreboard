# Making a slideshow kiosk on a Raspberry Pi

So first of all, this has nothing to do directly with the scoreboard.  But it
was a project I undertook for my local CSz theater and I thought it might be
good to publish how I did it.  Also I wanted notes for myself.

## The What

I wanted a Raspberry Pi that I could just plug into a projector that as soon as
boots up, loads a Google Slides show and auto-plays it.  The idea was to just
make the pre-show slide deck an application that no one has to set up or worry
about accidentally mousing over on top of while they prep for the show or
whatever.

If you like this idea but dislike parts of it (let's say you're not an Ubuntu
fan) That's fine!  If you have suggestions for alternate setups, feel free to
send a pull request to this doc or just email me with your version and I can
add alternate ways of doing the same thing.

## The steps

* Install Ubuntu on a Raspi using Ubuntu's guide:
https://ubuntu.com/tutorials/how-to-install-ubuntu-desktop-on-raspberry-pi-4#1-overview
   * This basically consists of installing the Raspberry Pi Imager and then
   going through the normal "first time boot" steps with a keyboard/mouse/monitor
   hooked up to the pi.  Then update all of the packages, because there will be
   day 1 updates.

* By default, Ubuntu's first account will be in the sudoers file.  So I leave
  that account with a secure password and make a second account that is not in
  the sudoers file to run the kiosk.

* At this point, I also find it's useful to give the machine an easy-to find
  hostname, because most maintenance will be done over SSH

* sudo apt install screen vim ssh lightdm openbox unclutter chromium-browser

* Sign in once as the kiosk user to set openbox as your wm

* To make the kiosk autologin, set the lightdm config (/etc/lightdm/lightdm.conf) to look like this:
  ```
  [Seat:*]
  autologin-guest=false
  autologin-user=kiosk
  autologin-user-timeout=0
  ```

### The Kiosk script
```
#!/bin/bash

export url='https://docs.google.com/presentation/d/e/<Slides Key>'

# Run this script in display 0 - the monitor
export DISPLAY=:0

# Turn off screensavers
xset -dpms s off

# Hide the mouse from the display -- turn this on if you have a mouse pointer hanging out over your slides.
#unclutter &

# If Chromium crashes (usually due to rebooting), clear the crash flag so we don't have the annoying warning bar
sed -i 's/"exited_cleanly":false/"exited_cleanly":true/' /home/kiosk/.config/chromium/Default/Preferences
sed -i 's/"exit_type":"Crashed"/"exit_type":"Normal"/' /home/kiosk/.config/chromium/Default/Preferences

until nslookup www.google.com; do
        echo No internet found, waiting for valid DNS...
        sleep 1
done

while (true) ; do
        echo Starting browser...
        /usr/bin/chromium-browser --kiosk --disable-infobars ${url} 
        #/usr/bin/firefox --kiosk ${url}
done
```

(I found firefox to be a bit more performant for a pi 3b, but Chromium looks
better on a 4. You should run your own tests and see which runs better for your slides)

### ~kiosk/.config/autostart/kiosk.desktop
```
[Desktop Entry]
Type=Application
Name=Kiosk
Exec=/home/kiosk/kiosk.sh
X-GNOME-Autostart-enabled=true
```

## Miscellaneous Notes

* I considered setting the box up to auto-update from apt in the background.
  I decided against this, as I don't want it to kick off an update during the
  pre-show slides and possibly degrade performance (or shut down the slides
  themselves.)  That does mean, however, that I'm on the hook for ensuring
  that the box gets updated periodically, for security.

* I started with a Raspi 3B and it _mostly_ worked, although some of the WebGL
transitions look a little bit janky.  A Raspi 4 works noticably better.