MainCtrl = function(app, model)
{
	DJ.Core.Ctrl.call(this, app, model); //extend base framework class
	
	/** Used to set up resources or values specific to this Ctrl / Model (and thus the entire Phase). */ 
	this.start = function() //abstract
	{
	};
	
	/** Used to clean up resources or reset values for this Ctrl, if it is no longer needed and can be released. */ 
	this.finish = function() //abstract
	{
	};
	
	/** Update simulation state by making changes to associated Model. */
	this.simulate = function(deltaSec) //abstract
	{
		var model = this.model;
		var monsters = model.monsters;
		var length = monsters.length;
		for (var m = 0; m < length; m++)
		{
			var monster = monsters[m];
			
			//move
			if (monster.status == MONSTER_STATUS_WAITING) //decide whether to move or wait
			{
				monster.status = Math.floor(Math.random() * 2); //even chance to continue waiting or begin moving 
				if (monster.status == MONSTER_STATUS_MOVING) //just began to act
				{
					//set new destination to begin moving to, next update
					monster.xNext = Math.floor(Math.random() * model.arenaWidth);
					monster.yNext = Math.floor(Math.random() * model.arenaHeight);
					var xDirection = monster.xNext - monster.x;
					var yDirection = monster.yNext - monster.y;
					var m = Math.sqrt(xDirection * xDirection + yDirection * yDirection);
					monster.xVelocity = monster.speed * xDirection / m;
					monster.yVelocity = monster.speed * yDirection / m;
				}
			}
			else //if acting, continue moving to destination
			{
				var xToDestination = monster.xNext - monster.x;
				var yToDestination = monster.yNext - monster.y;
				
				var distanceRemaining = Math.sqrt(xToDestination * xToDestination + yToDestination * yToDestination);
		
				if (distanceRemaining < monster.speed)
				{
					//monster.x = monster.xNext;
					//monster.y = monster.yNext;
					monster.status = MONSTER_STATUS_WAITING;
				}
				else
				{
					monster.x += monster.xVelocity * deltaSec;
					monster.y += monster.yVelocity * deltaSec;
				}
			}
		}
	};
};

MainCtrl.prototype = Object.create(DJ.Core.Ctrl.prototype);
MainCtrl.prototype.constructor = MainCtrl;
