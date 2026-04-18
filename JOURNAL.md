# Material Sourcing and BOM Creation

_Time_Spent_:
1hr

# Overview
In this session, I started by thinking up a suitable project. It didnt take that long for me to decide on this, well, mostly because its complexity is manageable for my level... for now...
It also addresses an issue i have with working with CAD software so its a win win situation.

The project will make use of the following:

### Raspberry pi PICO RP2040

![RASPBERRY PI](https://stasis.hackclub-assets.com/images/1776534806992-wemdos.png)

### OLED Display

![oled](https://stasis.hackclub-assets.com/images/1776534841668-nlq53y.png)

### Rotary Encoder

![enc](https://stasis.hackclub-assets.com/images/1776534937061-gyxy3q.png)

### Joystick Module

![JOYSTICK MODULE](https://stasis.hackclub-assets.com/images/1776534977262-3wuhfl.png)

### Buzzer

![buzz](https://stasis.hackclub-assets.com/images/1776535052324-nq854w.png)

### 800 Point Breadboard

![breadboard](https://stasis.hackclub-assets.com/images/1776535105889-afylhv.png)

- Jumper wires and push buttons are a part of this project but arent included in this BOM, since a previous project has covered those



# Wiring Diagram and Unit Testing

_Time_Spent_:
5hrs

# Overview
Since i had decided what parts are going to be in my project, i started working on the wiring diagram.
Even though this is my second submitted project here, it was actually my first attempt at creating a wiring circuit, so as you would imagine there were a lot of mistakes.

Although i made quite a funny mistake by putting all four pins of my joystick on a single column, had a headache trying to figure out why my joystick is mixing signals. I eventually went on a break, took a walk, and mid walk genuinely realized what i had done.

I also had issues with the buttons, cause i didnt understand which sides of the four pins were internally attached, so it didnt work for quite a while before i realized.

I tested each part individually to make sure everything was wired properly.
After i had tested everything, the encoder just randomly started registering double signals, but after learning about bounce and stuff, i was able to fix it

![CAD MacroPad Prototype 1](https://stasis.hackclub-assets.com/images/1776536126907-z7aubl.png)

After everything i went through in this stage i genuinely believed that this project could reach tier 3, but unfortunately i didnt meet the time requirement, and further thought made me realize its still only tier 2. Almost tier 3 though... i think



# CAD Design

_Time_spent_
3hrs

### Overview
I now had a clear parts list and wiring diagram, the only thing left was the building the 3D model

I used a lot of new features in fusion for this project. It was genuinely very exiciting, especially because i figured out how to use some very helpful features myself without a tutorial.

All the 3D models for the parts i used were gotten from GrabCAD, although getting the exact 3D models for the parts i used was kinda annoying, but i got them anyways.

Even though this has been my most complicated CAD build so far, it was still the most enjoyable

![3D Model 2](https://stasis.hackclub-assets.com/images/1776537311990-3q5ngd.png)

Its designed in a sort of console design, that would make it easier to hold.
And the screen is slightly angled so its more easily visible when placed on a flat surface

This the top view without the top cover

![CMP image 1](https://stasis.hackclub-assets.com/images/1776537536059-ggh7i5.png)

This is the view from the bottom with the underside of the case removed

![CMP image 2](https://stasis.hackclub-assets.com/images/1776537606265-3ks3ge.png)

And thats it!
