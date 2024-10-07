# elevator-scheduler-
scheduler algorithm 

My scheduler algorithm uses 2 ways to select elevators and assign them to people.
First it checks if a person is in the service range of the any elevator.
Second, it uses the first way of selecting an elevator suitable for a person. I choose this one at first because I wanted to use short distance to reach the person asking and after I realized it was too simple to use so I decided to implement a score based on criteria (like closest, waiting time after request, and current number in, and also direction of the elevator) and then the elevator with a best score will be given to the person. So it is an hybrid algorithm, it focus itself on choosing the best elevator based on distance, wait time since the elevator was requested,  number of person currently in the elevator, and direction suitable .
Then, I had some problems since some person could not be assigned to an elevator, so I created another selector function simpler like an alternative for people not selected in the first one, which is only based on short distance so the elevator with the minimum distance to reach the person start floor will be used. Also, it does not check elevator capacity . I put this when I did not have to check direction in my first scheduler but I am not sure if there is still people getting select by this alghorithm.
Then it returns the person id with the elevator matched, and it continues to assign till there is nobody left.

Problems encountered: when going to highrise bldg and when it is busy , there  are still people with no elevator match to them . I will probably plan on adding a 3rd alternate or maybe optimizing the other one.
