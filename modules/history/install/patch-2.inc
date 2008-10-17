<?php

  if (!$setup->columnExists("history","project"))
  {
    $setup->addColumn("history", "project", "int(10)", true, "0");
  }
