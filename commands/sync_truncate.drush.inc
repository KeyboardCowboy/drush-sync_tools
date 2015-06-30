<?php
/**
 * @file
 * Disables, uninstalls and reinstalls a module.
 */
/**
 * Command callback.
 */
function drush_sync_tools_truncate_table($table_names) {
  $truncate = array();
  $dne = array();

  // Resolve the site alias.
  $alias = drush_get_context(DRUSH_TARGET_SITE_ALIAS, '@self');
  $alias = empty($alias) ? '@self' : $alias;

  // Get the @self DB name.
  list($site_list,) = drush_sitealias_resolve_sitespecs(array($alias));
  $site = reset($site_list);
  drush_sitealias_add_db_settings($site);
  $db = $site['databases']['default']['default']['database'];

  // If we can't resolve a DB name, kick out.
  if (!$db) {
    return drush_set_error('NODB', dt('Unable to determine a database for this site.'));
  }

  // First, split the table string.
  $tables = explode(',', $table_names);
  foreach ($tables as $table) {
    // Check for wildcards and lookup all tables that match the pattern.
    if (strstr($table, '*')) {
      $table_like = str_replace('*', '%', $table);
      $result = db_query("SELECT TABLE_NAME FROM INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = :db AND TABLE_NAME LIKE :table;", array(':db' => $db, ':table' => $table_like));

      foreach ($result as $row) {
        $truncate[] = $row->TABLE_NAME;
      }
    }
    else {
      if (!db_table_exists($table)) {
        $dne[] = $table;
      }
      else {
        $truncate[] = $table;
      }
    }
  }

  // If there are erroneous tables report them.
  if (!empty($dne)) {
    $error = drush_set_error('TABLE_DNE', dt('The following tables do not exist on database @db and cannot be truncated.', array('@db' => $db)));
    drush_print_format($dne, 'list');
    return $error;
  }

  // Print a list of tables to be truncated.
  drush_print_format($truncate, 'list');

  // Make sure the user is cool with this.
  if (!drush_confirm(dt("You are about to truncate !num tables on !db. Continue?", array('!num' => count($truncate), '!db' => $db)))) {
    return drush_user_abort();
  }
  else {
    foreach ($truncate as $table) {
      db_query("TRUNCATE TABLE {{$table}}");
      drush_print(dt('Truncated @table', array('@table' => $table)));
    }
  }
}
