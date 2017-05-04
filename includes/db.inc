<?php

/**
 * @file
 * Islandora Default Thumbs database wrapper functions.
 */

function islandora_default_thumb_get_node_config($content_models = NULL) {
  $result;
  if ($content_models) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'islandora_default_thumbnail')
    ->propertyCondition('status', 1)
    ->fieldCondition('field_content_model_s_ ', 'value', $content_models, 'IN')
    ->addMetaData('account', user_load(1)); // Run the query as user 1.
    $result = $query->execute();
  }

  $entities;
  if (isset($result['node'])) {
    $thumbnail_configs = array_keys($result['node']);
    $entities = entity_load('node', $thumbnail_configs);
  }

  $config = array();
  if (isset($entities)) {
    $field_replace_info = field_info_instance('node', 'field_missing_thumbnail_replace', 'islandora_default_thumbnail');
    foreach ($content_models as $cmodel) {
      foreach ($entities as $nid => $node_values) {
        $config[$cmodel] = array(
          'thumb_replace' => isset($node_values->field_missing_thumbnail_replace['und'][0]['uri']) ? $node_values->field_missing_thumbnail_replace['und'][0]['uri'] : "",
          'classes'       => isset($node_values->field_thumbnail_css_classes['und'][0]['value']) ? $node_values->field_thumbnail_css_classes['und'][0]['value'] : "",
          'image_style'   => isset($field_replace_info['display']['default']['settings']['image_style']) ? $field_replace_info['display']['default']['settings']['image_style'] : "",
          'image_detect'  => isset($node_values->field_replacement_images_detect['und']) ? islandora_default_thumb_file_hash($node_values->field_replacement_images_detect['und']) : "",
          'image_replace' => isset($node_values->field_replacement_images_replace['und'][0]['uri']) ? file_create_url($node_values->field_replacement_images_replace['und'][0]['uri']) : "",
        );
      }
    }
  }

  return $config;
}

function islandora_default_thumb_file_hash($replacements) {
  $file_hashs = array();
  foreach ($replacements as $key => $file_info) {
    $file_hashs[] = isset($file_info['filehash']['md5']) ? $file_info['filehash']['md5'] : NULL;
  }
  return $file_hashs;
}
