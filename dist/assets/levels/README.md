# Level file format

Each point on map is represented by `[...]`, where `...` are one of the below. Rows are y-axis, columns – x-axis. Whitespace characters are ignored (inside and outside of brackets).

![]( ../../../gfx/metalTileConnectStraight_NE.png) – "vertical track" – `[|]`

![]( ../../../gfx/metalTileConnectStraight_NW.png) – "horizontal track" – `[-]`

![]( ../../../gfx/metalTileConnectCornerInner_SE.png) – "up-right turn" – `[\_]`

![]( ../../../gfx/metalTileConnectCornerInner_NW.png) – "left-up turn" – `[_/]`

![]( ../../../gfx/metalTileConnectCornerInner_SW.png) – "down-right turn" – `[/~]`

![]( ../../../gfx/metalTileConnectCornerInner_NE.png) – "left-down turn" – `[~\]`


## Special modifiers

Vertical and horizontal tracks can be set to be a start or finish point (only one of each): `[|s]`, `[|f]`, `[-s]`, `[-f]`, which will produce (based on context):

![]( ../../../gfx/metalTileConnectEnd_SE.png)
![]( ../../../gfx/metalTileConnectEnd_NE.png)
![]( ../../../gfx/metalTileConnectEnd_NW.png)
![]( ../../../gfx/metalTileConnectEnd_SW.png)
