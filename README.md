# OW3.0
This project was submitted as my final for USC's TAC438, Advanced Gameplay Programming. Inspired by my own decade of playing Blizzard's Overwatch, I tasked myself with developing a multiplayer, team-based game.

# Notable Features
## Gameplay:
- supports multiple maps and saves
- supports capture and deathmatch game modes
- spawn immunity
  - players can shoot but not take damage
- best of 3
  - implemented with game instance subsystem to preserve data across rounds
  
## Technical:
- imported FAB content and graphic polish
- looped sequences to move objects on the map to make it more difficult to shoot opponents
  - implemented with multicast to avoid delays
  - sound effects for objects in sequences
  - preserves team points across rounds
 
##  Capture UI:
- counter to display how many points a team has from winning rounds
- indicator of player(s) contesting the point
  - only appears if both teams have players on the point
  - counter to display how many players of each team are on the point
