Utility = 
{
	//ES5
	bind: function( scope, fn )
	{
		return function ()
		{
			fn.apply( scope, arguments );
		};
	}
}