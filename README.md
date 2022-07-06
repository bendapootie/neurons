# neurons
Playing around with neural networks

```mermaid
flowchart LR
	i0((In 0))
	i1((In 1))
	o0((Out 0))
	o1((Out 1))
	o2((Out 2))
	o3((Out 3))
	network0((...))
	network1((...))
	network2((...))
	i0-->network0
	i1-->network0
	network0-->network1
	network1-->network2
	network2-->o0
	network2-->o1
	network2-->o2
	network2-->o3
```

```mermaid
flowchart LR
	ip0((P0 State))
	ip1((P1 State))
	ib((Ball State))
	
	oP00((P0 Steer))
	oP01((P0 Speed))
	
	network0[Neural Network]
	
	ip0-->network0
	ip1-->network0
	ib-->network0
	
	network0-->oP00
	network0-->oP01
```

| Inputs               | Description |
| -------------------- | :---------: |
| Player X             | [0 .. Field Width]  |
| Player Y             | [0 .. Field Length] |
| Player Facing Angle  | [0 .. 360]          |
| Player Velocity X    | [-Max Speed .. +MaxSpeed] |
| Player Velocity Y    | [-Max Speed .. +MaxSpeed] |
| Enemy X              | [0 .. Field Width]  |
| Enemy Y              | [0 .. Field Length] |
| Enemy Facing Y       | [0 .. 360]          |
| Enemy Velocity X     | [-Max Speed .. +MaxSpeed] |
| Enemy Velocity Y     | [-Max Speed .. +MaxSpeed] |
| Ball X               | [0 .. Field Width]  |
| Ball Y               | [0 .. Field Length] |
| Ball Velocity X      | [-Max Speed .. +MaxSpeed] |
| Ball Velocity Y      | [-Max Speed .. +MaxSpeed] |

| Outputs              | Description |
| -------------------- | :---------: |
| Player Steering      | [Max Left .. Max Right]  |
| Player Speed         | [Max Reverse .. Max Forward] |
