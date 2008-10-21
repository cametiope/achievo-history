<?php
  $setup->alterColumn( 'organization', 'parent_id', 'parent_id', 'int(11)', true);
  $setup->alterColumn( 'organization', 'manager_id', 'parent_id', 'int(11)', true);
