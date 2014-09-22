disjunction.extensions.Array2 = function(xLength, yLength)
{
	this.set = function(xi, yi, element)
	{
		return (this.array[xi + yi * this.xLength] = element);
	}
	
	this.get = function(xi, yi)
	{
		return this.array[xi + yi * this.xLength];
	}
	
	this.resize = function(xLength, yLength)
	{
		this.xLength = xLength;
		this.yLength = yLength;
		var temp = this.array;
		var array = this.array = new Array(xLength * yLength);
		
		//console.log('length=', array.length);
		var limit = array.length < temp.length ? array.length : temp.length;
		for (var i = 0; i < limit; i++)
		{
			array[i] = temp[i];
		}
	}
	
	this.array = [];
	this.xLength = 0;
	this.yLength = 0;
	this.resize(xLength, yLength);
};