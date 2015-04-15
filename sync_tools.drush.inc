<?php

/**
 * Implements hook_drush_command().
 *
 * @return mixed
 */
function sync_tools_drush_command() {
  $items['sync-to-local'] = array(
    'description' => 'Sync a DB and run the necessary commands to prep a local environment.',
    'bootstrap' => DRUSH_BOOTSTRAP_NONE,
    'arguments' => array(
      'source-alias' => 'The source alias to fetch the DB from.',
    ),
    'options' => array(
      'skip-git' => 'Do not execute a git pull before running the reset commands.',
      'no-cache' => 'Ignore any cached versions of the database and do a fresh pull.',
      'no-revert' => 'Do not revert features.',
    ),
    'required-arguments' => TRUE,
    'aliases' => array('stl'),
  );

  $items['reset'] = array(
    'description' => 'Batch command to run DB updates, revert features, clear caches.',
    'bootstrap' => DRUSH_BOOTSTRAP_NONE,
    'options' => array(
      'skip-git' => 'Do not execute a git pull before running the reset commands.',
      'no-revert' => 'Do not revert features.',
    ),
  );

  return $items;
}
