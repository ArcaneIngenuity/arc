disjunction.core.Point2 = function(x, y)
{
	this.x = x || 0;
	this.y = y || 0;
	
	this.clone = function()
	{
		return new disjunction.core.Point2(this.x, this.y);
		 clone;
	}
};

disjunction.core.Point2.prototype.constructor = disjunction.core.Point2; //TODO remove?