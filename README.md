# GOAP Research (Goal-Oriented Action Planning)
Topic: using GOAP (Goal Oriented Action Planning) in an AI that tries to survive against zombies in a small game (the given exam zombie game) for as long as possible.

GOAP is a decision making structure used for AI, similar to a finite state machine, with the biggest difference being that its more flexible.
You create actions that the AI can perform, and those actions have a precondition which needs to be met before they can be called, and they have an effect on the world.
Putting all of the actions into a planner, this will create the most optimal path (based on the actions' costs) from your current state towards the goal state you want to achieve.

![ActionGoapExample](https://user-images.githubusercontent.com/120639013/213436080-6d9f0cdc-9ba6-45bb-905e-a03991eca157.png)

To use this sysyem in a game (in this case, the zombie survival game, see files) we only need to put in all the actions we want our AI to be able to do.
For example: 
-Exploring the world
-Finding houses
-Looting houses
-Collecting items
-Using items
-Killing enemies
-Running from danger
-etc...

Every action also has a precondition, a state in which the world must be for the AI to go into the new action,
and an effect, which will leave the world in the new state after the action is complete.
This makes it so that all the actions fit together like domino pieces, and all the plannner has to do, is find the correct (and shortest/best) path towards the endgoal.

![dominoExplanation](https://user-images.githubusercontent.com/120639013/213439640-7dca1086-fb62-489f-bc64-c7eaeec78e37.png)


