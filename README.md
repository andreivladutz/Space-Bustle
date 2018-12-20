# Space-Bustle
Space Bustle is an Arduino arcade game with spaceships, meteors, flying bad guys and a lot of ammunition to light up the dark skies of outer space.
## Setup
- Arduino Uno<br/>
  *the microcontroller with the magic*
- 8x8 LED Matrix with MAX7219 driver<br/>
  *where the bustle happens*
- LCD display<br/>
  *used for displaying player lives, level and score*<br/>
  *also used for telling you the game is paused or over (you've lost, duh)*
- Joystick<br/>
  *used for doging bullets, meteors, and all kinds of bad objects falling from the sky (a.k.a. controlling the main player :D)*
- Button<br/>
  *used for playing/pausing/restarting the game when u dead*
  
 ## The Code
 The code is split in multiple headers and cpp's each one with its own responsibility :
- **screenRenderer**<br/>
  *this class is responsible for drawing pixels on the matrix, setting, clearing and all kinds of stuff*<br/>
  *also printing messages and heart characters on the LCD*
- **controlInterface**<br/>
  *this class handles all the input -> the joystick, returning movement codes for left, right, up and down<br/>
                                    -> joystick clicking, attaching an interrupt<br/>
                                    -> game button clicking, attaching an interrupt for it too*
- **Actor**<br/>
  *here we keep all the logic for our game objects, having a hierarchy of classes*<br/>
  ENTITY:point_right:BULLET<br/>
  :point_down:<br/>
  ACTOR:point_right:ENEMY<br/>
  :point_down:<br/>
  PLAYER<br/>
  <br/>
  - *Meteors objects will be of type ENTITY*
  - *Bullets will be of type BULLET*
  - *The player will be of type, ..well.., PLAYER*
  - *Enemies -> ENEMY*
- **EntityArray**<br/>
  *a little utilty -> a safe array of entities used for keeping bullets and meteors*<br/>
  *the actors(Player and Enemies) both can shoot, each one has to keep track of their bullets to know if they hit the right guy*<br/>
  *also, the gameInterface has to keep track of the meteors it spawns*
- **gameInterface**<br/>
  *the guy that takes care of everything -> spawning the player, updating positions, checking collisions, pausing, restarting, dropping meteors out of the sky, trying to kill the player with tougher, faster, enemies, etcetera, etcetera.*<br/>
  <br/>
The game has two playing states <br/>
-> Meteor Shower which ends after a duration (this duration increases every level)<br/>
-> Enemy Fight (it spawns a nice enemy, well, actually not that nice to the player) where the player has to dodge the enemy's bullets and defeat him (by shooting back, or, if crazy enough, colliding with him until it blows up)<br/>
<br/>
Once an enemy is defeated you get a higher level, some nice rewarding score points and... you go again, this time with increased difficulty.<br/>
link:https://youtu.be/QpzEUyjG-nk[link to video]
