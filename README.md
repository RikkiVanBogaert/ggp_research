# GOAP Research (Goal-Oriented Action Planning)
Topic: using GOAP (Goal Oriented Action Planning) in an AI that tries to survive against zombies in a small game (the given exam zombie game) for as long as possible.

Intro:
GOAP is a decision making structure used for AI, similar to a finite state machine, with the biggest difference being that its more flexible.
You create actions that the AI can perform, and those actions have a precondition which needs to be met before they can be called, and they have an effect on the world.
Putting all of the actions into a planner, this will create the most optimal path (based on the actions' costs) from your current state towards the goal state you want to achieve.
