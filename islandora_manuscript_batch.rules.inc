<?php

/**
 * @file
 * Rules hook implementations.
 */

/**
 * Implements hook_rules_event_info().
 */
function islandora_manuscript_batch_rules_event_info() {
  $events = array();

  $events['islandora_manuscript_batch_ingested'] = array(
    'group' => t('Islandora Manuscript'),
    'label' => t('Manuscript ingested'),
    'variables' => array(
      'manuscript_object' => array(
        'type' => 'islandora_object',
        'label' => t('The ingested manuscript'),
        'description' => t('A Tuque object for the ingested manuscript, as an entity.'),
      ),
    ),
  );

  return $events;
}
