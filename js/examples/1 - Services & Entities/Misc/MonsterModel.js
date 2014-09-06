MonsterModel = function()
{
	this.name = undefined;
	this.index = 0;
	this.x = undefined;
	this.y = undefined;
	this.xNext = undefined;
	this.yNext = undefined;
	this.xVelocity = 0;
	this.yVelocity = 0;
	this.status = MONSTER_STATUS_WAITING;
	this.width = undefined;
	this.height = undefined;
}

MONSTER_STATUS_WAITING = 0;
MONSTER_STATUS_MOVING = 1;