{foreach from=$tabmenu item=menuitem}
	<li><img src="{$menuitem.img}"><a href="#" id="node_{$menuitem.id}" onclick="window.open('{$menuitem.url}','main','')" onmouseover="this.style.cursor = 'pointer'; this.style.color = '#9a1010';" onmouseout="this.style.color = '#414141';">{$menuitem.name}</a>
    {if $menuitem.sub}
  	  <ul>
  			<li parentId="{$menuitem.id}" parentType="{$menuitem.type}"><a href="#" id="sub_{$menuitem.id}">Loading...</a></li>
  		</ul>
  	{/if}
	</li>
{/foreach}