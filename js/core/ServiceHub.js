ServiceHub = function()
{
	this.array = [];
	
	this.get = function(i)
	{
		return this.array[i];
	}
	
	this.add = function(service)
	{
		var array = this.array;
		array.push(service);
		
		return array.length - 1;
	}
	
	this.dispose = function()
	{
		var array = this.array;
		for (var i = 0; i < array.length; i++)
		{
			array[i].dispose();
		}
	}
}