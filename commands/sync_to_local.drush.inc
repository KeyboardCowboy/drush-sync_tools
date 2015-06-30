<?php
/**
 * @file
 * Disables, uninstalls and reinstalls a module.
 */
/**
 * Command callback.
 */
function drush_sync_tools_sync_to_local($source) {
  $response = NULL;

  // Get information about the @self alias so we can ask the user to confirm.
  list($site_list,) = drush_sitealias_resolve_sitespecs(array('@self'));
  $self = reset($site_list);
  drush_sitealias_add_db_settings($self);
  $db = $self['databases']['default']['default']['database'];

  // Report if unable to find a DB to sync into.
  if (empty($db)) {
    drush_set_error('STL-NODB', 'Either you are not within a Drupal site or the default database is not defined.');
    return FALSE;
  }

  // Prompt the user to confirm before beginning.
  while (!in_array($response, array('y','n'))) {
    $response = drush_prompt("This will wipe out your local database '{$db}' and sync it from {$source}. Continue? (y/n)");
  }

  // Kick out
  if ($response == 'n') {
    drush_print('Aborting local sync.');
    return;
  }

  // Sync the DB
  $commands['sql-sync'] = array(
    'command' => 'sql-sync',
    'arguments' => array('source' => $source, 'target' => '@self'),
    'options' => array(
      'yes' => TRUE,
      'create-db' => TRUE,
      'sanitize' => TRUE,
      'sanitize-email' => 'no',
    ),
  );

  $commands['syncdb'] = array(

  );

  // Allow the --no-cache option to be passed in for sql-sync
  if (drush_get_option('no-cache', FALSE)) {
    $commands['sql-sync']['options']['no-cache'] = TRUE;
  }

  // Run the standard reset
  $commands['reset'] = array(
    'command' => 'reset',
    'options' => array(
      'no-revert' => drush_get_option('no-revert'),
    ),
  );

  // Apply the local settings
  // Requires: drush-denver
  $commands['env'] = array(
    'command' => 'env',
    'arguments' => array('local'),
    'options' => array('yes' => TRUE),
  );

  // Clear all caches.
  $commands['cache-clear'] = array(
    'command' => 'cache-clear',
    'arguments' => array('all'),
  );

  // Pass the drushrc file through to drush_invoke_process
  $default_options = array();
  if ($config = drush_get_option('config-file')) {
    $default_options['config'] = $config;
  }

  // Pull the latest changes from the git branch
  if (!drush_get_option('skip-git')) {
    drush_shell_exec('git pull');
    foreach (drush_shell_exec_output() as $msg) {
      drush_print($msg);
    }
  }

  // Run the commands.
  foreach ($commands as $command) {
    $command += array('alias' => '@self', 'arguments' => array(), 'options' => array());
    $command['options'] += $default_options;

    drush_invoke_process($command['alias'], $command['command'], $command['arguments'], $command['options']);
    if (drush_get_error() != DRUSH_SUCCESS) {
      return drush_set_error('LOCAL_SYNC_FAILED', dt('Sync failed on drush @command.', array('@command' => $command['command'])));
    }
  }

  // Success!
  drush_log(dt('Local sync completed.'), 'success');
}
