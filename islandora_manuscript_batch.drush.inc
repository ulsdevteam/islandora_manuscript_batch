<?php

/**
 * @file
 * Implementation of Drush hooks.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_manuscript_batch_drush_command() {
  $items = array();

  $items['islandora_manuscript_batch_preprocess'] = array(
    'aliases' => array('imbp'),
    'description' => 'Preprocessed manuscripts into database entries.',
    'drupal dependencies' => array('islandora_batch',
      'islandora_manuscript',
      'islandora_manuscript_batch'),
    'options' => array(
      'type' => array(
        'description' => 'Either "directory" or "zip".',
        'required' => TRUE,
      ),
      'scan_target' => array(
        'description' => 'The target to directory or zip file to scan.',
        'required' => TRUE,
      ),
      'namespace' => array(
        'description' => 'The namespace for objects created by this command.  Defaults to namespce set in fedora config.',
        'required' => FALSE,
      ),
      'content_models' => array(
        'description' => 'A comma-separated list of content models to assign ' .
        'to the objects. Only applies to the "manuscript" level object.',
        'value' => 'optional',
      ),
      'parent' => array(
        'description' => 'The collection to which the generated items should ' .
        'be added.  Only applies to the "manuscript" level object. If ' .
        '"directory" and the directory containing the manuscript description is ' .
        'a valid PID, it will be set as the parent. If this is specified ' .
        'and itself is a PID, all manuscripts will be related to the given PID.',
        'value' => 'optional',
      ),
      'parent_relationship_uri' => array(
        'description' => 'The namespace URI of the relationship to the parent.' .
        ' Defaults to "info:fedora/fedora-system:def/relations-external#".',
        'value' => 'optional',
      ),
      'parent_relationship_pred' => array(
        'description' => 'The predicate of the relationship to the parent. ' .
        'Defaults to "isMemberOfCollection".',
        'value' => 'optional',
      ),
      'create_pdfs' => array(
        'description' => 'A flag to cause PDFs to be created in manuscripts. Page PDF creation is dependant on the configuration within Drupal proper.',
        'value' => 'optional',
      ),
      'do_not_generate_ocr' => array(
        'description' => 'A flag to allow for conditional OCR generation.',
        'value' => 'optional',
      ),
      'email_admin' => array(
        'description' => 'A flag to notify the site admin when the manuscript is ' .
        'fully ingested (depends on Rules being enabled).',
        'value' => 'optional',
      ),
      'wait_for_metadata' => array(
        'description' => 'A flag to indicate that we should hold off on ' .
        'trying to ingest manuscripts until we have metadata available for them' .
        'at the manuscript level.',
        'value' => 'optional',
      ),
      'directory_dedup' => array(
        'description' => 'A flag to indicate that we should avoid ' .
        'repreprocessing manuscripts which are located in directories.',
      ),
      'page_progression' => array(
        'description' => 'A flag to indicate the page progression for the manuscript. If not specified will default to LR.',
        'value' => 'optional',
      ),
      'overwrite_existing' => array(
        'description' => 'If set, the system will check if object exists and purge it ' .
        'before ingesting.  In all other cases, ingesting a batch that includes any ' .
        'existing objects would fail.  NOTE:  Objects would be purged at the point in time ' .
        'when a batch is scanned for ingest, not when the actual ingest set is processed.',
        'value' => 'optional',
      ),
      'assign_pid' => array(
        'description' => 'The PID value (including namespace) to assign to the object. If this value ' .
        'is provided, the namespace value is ignored.',
        'value' => 'optional',
      ),
      'relationship' => array(
        'description' => 'An additional relationship to create.',
        'value' => 'optional',
      ),
      'relationship_pred' => array(
        'description' => 'The predicate of the relationship. ',
        'value' => 'optional',
      ),
      'relationship_namespace' => array(
        'description' => 'Namespace of additional relationship.',
        'value' => 'optional',
      ),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );

  return $items;
}

/**
 * Implements hook_drush_command().
 */
function drush_islandora_manuscript_batch_preprocess() {
  // XXX: Due to how Drush bootstrapping works, the connection may be created
  // without credentials (when your site's front page is
  // 'islandora/object/some:object', for example). Resetting to ensure a new
  // connection gets created should fix it.
  drupal_static_reset('islandora_get_tuque_connection');
  $connection = islandora_get_tuque_connection();

  module_load_include('inc', 'islandora_batch', 'includes/common');
  $base_parameters = islandora_batch_get_base_drush_parameters();
  $manuscript_batch_parameters = array(
    'create_pdfs' => drush_get_option('create_pdfs', FALSE),
    'email_admin' => drush_get_option('email_admin', FALSE),
    'wait_for_metadata' => drush_get_option('wait_for_metadata', FALSE),
    'directory_dedup' => drush_get_option('directory_dedup', FALSE),
    'page_progression' => drush_get_option('page_progression', FALSE),
  );
  $parameters = array_merge($base_parameters, $manuscript_batch_parameters);

  if ($content_models = drush_get_option('content_models', FALSE)) {
    $parameters['content_models'] = explode(',', $content_models);
  }
  else {
    $parameters['content_models'] = array('islandora:manuscriptCModel');
  }

  if ($do_not_generate = drush_get_option('do_not_generate_ocr', FALSE)) {
    $parameters['generate_ocr'] = FALSE;
  }
  else {
    $parameters['generate_ocr'] = module_exists("islandora_ocr");
  }

  $preprocessor = new IslandoraManuscriptBatch($connection, $parameters);

  // Pass the preprocessor off to run.
  $preprocessed = islandora_batch_handle_preprocessor($preprocessor);
}

/**
 * Manuscript batch preprocess validation.
 */
function drush_islandora_manuscript_batch_preprocess_validate() {
  $page_progression = drush_get_option('page_progression', FALSE);
  if ($page_progression) {
    if ($page_progression != 'lr' && $page_progression != 'rl') {
      return drush_set_error('Invalid page progression parameter.', dt('The specified page progression of @progression is not valid. Only the values "lr" or "rl" can be used.', array('@progression' => $page_progression)));
    }
  }
}
