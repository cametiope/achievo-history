//get_object is a function in atktools.js    
function openObject(tablename,regexpression,imageopen,imageclose)
{
  theRows     = get_object(tablename).rows;
  imgOpenObj  = get_object(imageopen);
  imgCloseObj = get_object(imageclose);
      
  reg=new RegExp(regexpression);
  
  //test all rows of the table.
  for (i=0;i<theRows.length;i++)
  {
    //if we find one that passes the test
    if (reg.test(theRows[i].id))
    {          
      imgOpenObj.style.display='none';
      imgCloseObj.style.display='';
      theRows[i].style.display='';          
    }
  }      
}

function closeObject(tablename,regexpression,imageopen,imageclose)
{
  theRows = get_object(tablename).rows;      
  imgOpenObj  = get_object(imageopen);
  imgCloseObj = get_object(imageclose);                 
      
  reg=new RegExp(regexpression);
  
  //test all rows of the table.
  for (i=0;i<theRows.length;i++)
  {
    //if we find one that passes the test
    if (reg.test(theRows[i].id))
    {
      imgOpenObj.style.display='';
      imgCloseObj.style.display='none';
      theRows[i].style.display='none';       
    }
  }      
}