# Multi-Robot Persistent Monitoring

_This repository contains a partial implementation of the project. Full code will be released upon completion of the project._

In this project, we study the multi-robot visibility-based persistent monitoring (PM) problem. Persistent monitoring is a variant of coverage problem that involves repeatedly covering a given area, e.g. patrolling, security surveillance, target searching, etc. In addition to the coverage maximization objective of the well-studied exploration problem, the PM problem also requires agents to revisit previously explored regions in order to achieve repeated monitoring of the space. More specifically, in this work, we study a novel visibility-based PM problem where a team of UGVs equipped with 360◦ visual sensors repeatedly patrols an obstructed 2D region. We call the problem visibility-based persistent monitoring (VPM) problem. 

We propose novel algorithms to plan the path of multiple UGVs to solve the VPM problem. The design of an algorithm that plans the motion of the UGVs performing the VPM task depends on whether each UGV knows the position of all the other UGVs. In a practical scenario, it might be unrealistic to assume that each UGV always knows the positions of all other UGVs exactly, which requires uninterrupted communication among all the UGVs. In this work, we make a more realistic assumption that the UGVs might not always maintain communication with one another, hence have incomplete information about the position of the other UGVs.

Our solution to the VPM problem uses the idea of [Particle Filters](https://en.wikipedia.org/wiki/Particle_filter) to model the uncertainty in the position of other robots. We apply [Rapidly-exploring Random Tree (RRT)](https://en.wikipedia.org/wiki/Rapidly_exploring_random_tree) and Receding Horizon strategy to plan the paths of the UGVs. We conduct simulation-based experiments to demonstrate the effectiveness of our proposed algorithm.

