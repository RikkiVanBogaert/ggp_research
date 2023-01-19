# GOAP Research (Goal-Oriented Action Planning)
Topic: using GOAP (Goal Oriented Action Planning) in an AI that tries to survive against zombies in a small game (given exam zombie game) for as long as possible.

GOAP is a decision making structure used for AI, similar to a finite state machine, with the biggest difference being that its more flexible.
You create actions that the AI can perform, and those actions have a precondition which needs to be met before they can be called, and these actions in turn have an effect on the world.
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

##Explanation:
Every action also has a precondition, a state in which the world must be for the AI to go into the new action,
and an effect, which will leave the world in the new state after the action is complete.
This makes it so that all the actions fit together like domino pieces, and all the plannner has to do, is find the correct (and shortest/best) path towards the endgoal.

![dominoExplanation](https://user-images.githubusercontent.com/120639013/213439640-7dca1086-fb62-489f-bc64-c7eaeec78e37.png)
       
GOAP is like previously mentioned, very flexible.
Which means that if the current state changes, or if another external factor causes one of the states to not be achievable anymore,
the planner will adapt and find a new plan which is doable under the current conditions.
     
The planner can also receive multiple goals.
Each goal will have a different priotity, in order for the planner to try to make a plan for the most important goal first,
and if it cant find a plan, it moves on to the next goal.
     
##Current Implementation:    
In the current version of the game, not all aspects of GOAP have been implemented (yet).
Such as the cost-based plan finding, which allows the planner to find the cheapest goal based on the costs of each action in the plan.
Which will allow it to make multiple plans, and select the cheapest one to be executed.
This hasnt been implemented since the currently used game, is too simple in the sense that there arent multiple ways to get to a goal.
So finding one plan which lead to the current goal, will always be the best plan (if the actions have been implemented correctly).
     
Apart from this feauture, the GOAP-structure is reusable in different games.
The only things that have to be added manually every time, are the actions themselves, and the order of priorities in which the goals should be checked.
     
##Sources:   
https://alumni.media.mit.edu/~jorkin/goap.html    
https://gamedevelopment.tutsplus.com/tutorials/goal-oriented-action-planning-for-a-smarter-ai--cms-20793   
https://www.youtube.com/watch?v=tdBWk2OVCWc
