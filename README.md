# MOON-THEMED PHOTOREALISTIC 3D SCENE
## DESCRIPTION
$~~~$ A _photorealistic 3D scene_ using **OpenGL library**: The scene depicts the _surface of a moon-like planet_, located in the galaxy, the colourfulness of it therefore can be seen from the position of the user. The _terrain_ presents _craters_ like the moon and is mostly flat, excepting some uneven areas. \
$~~~$ The **main character** in the scene represents a _white-and-green robot_ that moves in the scene along with the camera (it cannot move beyond certain areas in order to not leave the ground). The **front view** of the scene presents the robot, together with a _tank_, that is _cameo-textured_ and its _aim_ is to **“kill” the robot**, and a _group of aliens_ that are _lit up by a streetlight pole_.

![image](https://github.com/ancamihai/Galaxy_Photorealistic_3D_Scene/assets/115399021/45bd4893-b4dd-4227-900b-9d4679da6ac2)

$~~~$ In the **right side** of the scene there exists a _rotating planet_ which ensembles Mars along with some _not-so-alive bushes_ to show that this planet is not suitable for living. \
$~~~$ The **back** represents the area where there are the _modes of transport_ for the creatures/robots on this planet. \
$~~~$ The **left side** presents some Star Wars characters – _the shiny C3PO_ and _the self-moving BB-8_, along with a _small grey cube_ which **generates orange balls**.

## FUNCTIONALITIES
1. When _opening the project_, the camera presents a **360 degrees view of the scene**, together with some pre-defined _to-the-left_, _to-the-right_, _forward_ and _backward trajectories_. \
   In addition, when using the _mouse_ or the _keyboard_, the _user can move around the scene_, the robot along with the view of the camera moving accordingly.
2. The user can _switch_ between _solid/smooth_, _wireframe/polygonal_ and _point_ **rendering of the scene** using the keyboard.
3. There are **multiple sources of light**: \
-> **directional light** (which _can be rotated_ with the keyboard), \
-> **pointlight** (_on the rocket_), \
-> **spotlight** (on the aliens),
both of _pointlight and spotlight_ being activated/deactivated with the keyboard.
4. The _floating planet_ **rotates by itself** and _BB-8_ **moves by itself** _except_ when the **main robot is in the Star-Wars-character’s surroundings**, when it stops moving.
5. When the _robot is near the grey cube_, an _orange ball_ **falls from the sky** and when it reaches the ground, it _can be picked by the robot_ and then the _robot can leave it again on the ground_. \
   This process can be repeated for very many times.
6. If the _robot is in the tank’s ray of action_, the _tank_ **starts moving** towards the robot and if the _distance becomes small enough_, the **tank makes the robot disappear**. \
   The _robot can be revived_ by the user and, in this regard, the _user can decide_ whether the **tank starts hunting the robot again or not**.
7. The user can activate/deactivate the **fog** by using the keyboard.

## USER MANUAL
$~~~$ To **run the application**: _double-click_ on _Project_GP.exe_ file \
$~~~$ When the _camera is animated_, the user _can’t interact with the scene at all_ (_except_ seeing the **depth map- by pressing M**). \
$~~~$ _After_ the camera animation is finished, the user can interact with the scene by _using the mouse to rotate around the scene_ (_up-down_ or _left-right_) or _use the keyboard_ in the following manner: \
$~~~~$ I. **For moving around the scene** 
1. _W_ – move camera/robot forward
2. _A_ – move camera/robot to the left
3. _S_ - move camera/robot backward
4. _D_ - move camera/robot to the right
5. _Q_ – rotate camera/robot to the left
6. _E_ – rotate camera/robot to the right
 
&nbsp;
$~~$ II. **For manipulating the light**
1. _Z_ – rotate the directional light to the right
2. _X_ – rotate the directional light to the left
3. _C_ – activate point light on the rocket
4. _V_ - deactivate point light on the rocket
5. _B_ – activate spot light on the group of aliens
6. _N_ - deactivate spot light on the group of aliens

&nbsp;
$~~$ III. **For effects** 
1. _F_ – make visible the shadows of the objects
2. _G_ – make non-visible the shadows of the object
3. _H_ – start rain
4. _J_ – stop rain
5. _K_ – activate fog effect
6. _L_ - deactivate fog effect

&nbsp;
$~~$ IV. **For rendering modes** 
1. _Y_ – solid
2. _U_ – wireframe
3. _I_ – point

&nbsp;
$~~$ V. **For actions in the scene**
1. _R_ – revive robot if dead
2. _T_ – restart tank’s hunting of the robot
3. _P_ – pick generated ball if possible
4. _O_ – leave on the ground the picked ball if possible

