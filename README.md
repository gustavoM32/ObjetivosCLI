# Objetivos (Goals)

Objetivos is a tool to increase productivity by setting Goal and the correspondent To-dos to reach them.

## Main features

* Creation of Goals that represent something to be done in the long term;
* Goals are structured in a hierarchical order, allowing for a Goal to be split in smaller ones;
* Creation of To-dos that represent what must be done to reach a Goal.
* Scheduling of To-dos to be completed;
* Monitoring of the time spent in specific To-dos and Goal to measure productivity for each day;
* Charts to analyse progress and to help decision-making.

## Detailed description

Below are more formal definitions of the structures in this program. They might be outdated.

### Goal

A Goal represents something to be done in the long-term.

It has the following attributes:

* **Name** - Goal name;
* **Code** - Code to easily identify a Goal that has 1 to 5 characters;
* **Motivation** - Why the Goal was created;
* **Notes** - Notes about the Goal evolution, for example, decisions about the change of course of a Goal.
* **State** - Current state of the Goal, that can be one of the following:
  * **Active** - When it is currently progressing;
  * **Inactive** - When it is not currently a priority;
  * **Completed** - When everything that was planned has been done;
  * **Canceled** - When it is not long necessary;
* **Sub-goals** - Smaller Goals that are inside a Goal. They are related to the completion of the parent Goal.
* **To-dos** - Actions to progress in the completion of a Goal
* **Sessions** - Periods of time dedicated to a To-do of a Goal.

### To-do

A To-do is an action to progress in the completion of a Goal.

It has the following attributes:

* **Description** - Description of what needs to be done;
* **Sub-to-dos** - Smaller To-dos to complete a bigger one.
* **State** - Uma tarefa pode estar em um dos seguintes estados:
  * **Pending** - When it has no priority to be done;
  * **Prioritized** - When it is prioritized to be done soon;
  * **Scheduled** - When it has a date to be done;
  * **Completed** - When it is completed.

A Session can be started from a To-do.

### Session

A Session is an interval of time to be dedicated for the completion of a To-do.

### Calendar

The Calendar helps to organize the Schedules of To-dos in different days and times. It is possible to estimate the time that will be dedicated for a To-do in a specific day.