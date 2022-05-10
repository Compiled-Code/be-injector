# be-injector
 
## How it works?
This works by abusing how Windows saves memory by re-using pages if they have not been modified, known as Copy on Write.  To evade Copy on Write, we obtain the physical address of the pages within the code section of the signed module, and patch directly to that.  When Windows loads the dll into the game, no Copy on Write violation would have taken place, thus it would load the dll with our patches

## How does this evade BattlEye?
BattlEye does not verify the integrity of the modules within the game.  PUBG, on its own, verifies the integrity of various system modules, but not loaded modules by the user.  Because the module we are loading is signed, BattlEye will blindly accept it.

## What doesit bypass?
- Thread checks
- Windows function calls
- Signature Scans

### Note
For this example, just the code section is mapped within the game.

![show](https://media.discordapp.net/attachments/958184678215528500/971602461808803910/unknown.png)

## Credits
- Xerox for VDM
