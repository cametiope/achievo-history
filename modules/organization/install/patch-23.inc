<?php
  $setup->alterColumn( 'organization', 'parent_id', 'parent_id', true);
  $setup->alterColumn( 'organization', 'manager_id', 'parent_id', true);
