<?php

/**
 * Implements hook_drush_command().
 *
 * @return mixed
 */
function sync_tools_drush_command() {
  $items['sync-to-local'] = array(
    'description' => 'Sync a DB and run the necessary commands to prep a local environment.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'arguments' => array(
      'source-alias' => 'The source alias to fetch the DB from.',
    ),
    'required-arguments' => TRUE,
    'aliases' => array('stl'),
  );

  $items['reset'] = array(
    'description' => 'Batch command to run DB updates, revert features, clear caches.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
  );

  return $items;
}
