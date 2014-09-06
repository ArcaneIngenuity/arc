MainCtrl = function(app, model)
{
	Ctrl.call(this, app, model); //extend base framework class
	
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
		for (var m in monsters)
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
				}	
			}
			else //if acting, continue moving to destination
			{
				//get direction of motion between current position and destination
				var motion = {x: monster.xNext - monster.x, y: monster.yNext - monster.y};
				//console.log('motion', motion);
				var m = Math.sqrt(motion.x * motion.x + motion.y * motion.y);
				//console.log('mag1', m);
				//normalize
				motion.x /= m;
				motion.y /= m;
				//console.log('mag2', Math.sqrt(motion.x * motion.x + motion.y * motion.y));
				 
				//multiply by velocity
				motion.x *= 50;
				motion.y *= 50;
				 
				//apply
				monster.x += motion.x * deltaSec;
				monster.y += motion.y * deltaSec;
				
				
				if (monster.x == monster.xNext && monster.y == monster.yNext) //at destination
				{
					monster.status = MONSTER_STATUS_WAITING;
				}
			}
		}
	};
};

MainCtrl.prototype = Object.create(Ctrl.prototype);
MainCtrl.prototype.constructor = MainCtrl;
