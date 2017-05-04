<?php

/**
 * @file
 * Islandora Default Thumbs Utility functions.
 */

function islandora_default_thumbs_get_result_set_cmodel_configs(&$variables, $result_set) {
  module_load_include('inc', 'islandora_default_thumbs', 'includes/db');

  islandora_default_thumbs_construct_common_objects_array($variables, $result_set);

  $cmodels = array();
  foreach($variables[$result_set] as $pid => &$values) {
    $values['cmodels'] = $cmodels = array_unique(array_merge($cmodels, $values['object']->{'models'}));
    $values['cmodels'] = $cmodels = array_diff($cmodels, array('fedora-system:FedoraObject-3.0'));

    $node_config = islandora_default_thumb_get_node_config($cmodels);
    foreach ($node_config as $cmodel => $cmodel_config) {
      $model = reset($values['cmodels']);
      if ($cmodel == $model) {
        $values['islandora_default_thumbs'] = $cmodel_config;
      }
    }
  }
}

// function islandora_default_thumbs_get_thumb_path(&$common, $node_config) {
//   $path;
//   //$datastream = isset($common['object'])$datastream->parent->id;
//   if (islandora_datastream_access(ISLANDORA_VIEW_OBJECTS, $datastream)) {
//     $pid =
//     $path = url("islandora/object/$pid/datastream/TN/view", array(
//       'absolute' => TRUE,
//       'language' => language_default(),
//     ));
//   }
//   else {
//     $logo_path = drupal_get_path('module', 'islandora_social_media_thumbnail') . '/image/islandora_logo.png';
//     $path = file_create_url($logo_path);
//   }

//   // Path to local copy of the datastream image.
//   $uri = imagecache_external_generate_path($path);

//   // Path to image style generated from the local datastream image.
//   $path = image_style_url($style, $uri);
//   // Return the path to the styled imagecache external resource.
//   return $path;
// }

function islandora_default_thumbs_style_image(&$value, $replace = FALSE) {
  $style = $value['islandora_default_thumbs']['image_style'];
  $path = $value['islandora_default_thumbs']['thumb_replace'];
  if ($replace) {
    $path = $value['islandora_default_thumbs']['image_replace'];
  }
  else {
    $path = file_create_url($path);
  }
  $uri = imagecache_external_generate_path($path);
  $styled_url = image_style_url($style, $uri);
  $image = theme('image', array('path' => $styled_url, 'alt' => $value['title']));

  $value['thumbnail'] = l(
    $image,
    "islandora/object/" . $value['object']->id,
    array(
      'html' => TRUE,
      'attributes' => array(
        'title' => $value['title'],
        'class' => explode(" ", $value['islandora_default_thumbs']['classes']),
      ),
    )
  );

  $value['thumb_link'] = $value['thumbnail'];

  $value['title_link'] = l(
    $value['title'],
    "islandora/object/" . $value['object']->id
  );
}

/**
 * Prepare page variables for use in page templates.
 *
 * @param array $variables
 *   Template preprocess provided variables array.
 */
function islandora_default_thumbs_prepare_vars_for_page_templates(&$variables, $result_set) {
  islandora_default_thumbs_get_result_set_cmodel_configs($variables, $result_set);
  $theme_default = TRUE;

  foreach ($variables[$result_set] as $key => &$value) {
  	if (isset($value['islandora_default_thumbs'])) {
      // Replace missing thumbnail.
//       if (!isset($value['object']['TN']) && isset($value['islandora_default_thumbs']['thumb_replace'])) {
//         islandora_default_thumbs_style_image($value);
//         $theme_default = FALSE;
//       }
//       // Compare and replace.
//       if (isset($value['islandora_default_thumbs']['image_detect'])) {
//         islandora_default_thumbs_replace_image_detect($value);
//         $theme_default = FALSE;
//       }
      // Theme existing thumbnail.
      if ($theme_default) {
        dsm($value, "on to style image");
        //$value['thumbnail'] = url("islandora/object/$key/datastream/TN/view", array('absolute' => TRUE));
        islandora_default_thumbs_style_image($value);
      }
    }
  }
}

function islandora_default_thumbs_replace_image_detect(&$value) {
  if (isset($value['object']['TN'])) {
    $hash = reset($value['islandora_default_thumbs']['image_detect']);
    $islandora_md5 = md5($value['object']['TN']->{'content'});

    if ($hash == $islandora_md5) {
      islandora_default_thumbs_style_image($value, TRUE);
    }
  }
}

function islandora_default_thumbs_construct_common_objects_array(&$variables, $result_set) {
//   if (!isset($variables['result_set'])) {
    //$variables['result_set'] = array();
    foreach ($variables[$result_set] as $key => &$value) {
       // dsm($value, "value");
      $pid = isset($value['pid']) ? $value['pid'] : $value['PID'];
      $label = isset($value['title']) ? $value['title'] : $value['label'];
      $object = isset($value['object']) ? $value['object'] : NULL;
      $variables[$result_set][$pid]['islandora_default_thumbnail'] = array(
        'object' => $object,
        'title' => $label,
        'thumbnail' => "",
        'cmodels' => $object->models,
      );
    }
//   }

  dsm($variables[$result_set], "result set stuff");
}

/**
 * Prepare variables for the solr templates.
 *
 * @param array $variables
 *   Template preprocess provided variables array.
 * @param string $result_set
 *   Key in the $variables array indicating result set.
 * @param string $view_type
 *   Either:
 *     standard => list or grid item views,
 *     search   => Search results page,
 *     compound => Compound object view.
 */
function islandora_default_thumbs_prepare_vars_for_solr_page_template(&$variables, $result_set, $view_type) {
  module_load_include('inc', 'islandora_default_thumbs', 'includes/db');
 // islandora_default_thumbs_construct_common_objects_array($variables, $result_set);
  islandora_default_thumbs_prepare_vars_for_page_templates($variables, $result_set);
 // dsm($variables, "vars in solr page temp prep");
 // dsm($view_type, "view type");
//   $count = 0;
//   foreach ($variables['associated_objects_array'] as $key => &$value) {
//     switch ($view_type) {
//       case 'standard':
//         if (!empty($value['thumbnail'])) {
//           $variables['objects'][$count]['thumb'] = $value['thumbnail'];
//         }
//         break;

//       case 'search':
//         if (!empty($value['thumbnail'])) {
//           $variables['results'][$count]['thumbnail'] = $value['thumbnail'];
//         }
//         break;

//       case 'compound':
//         if (!empty($value['full_config']['thumb'])) {
//           $variables['themed_siblings'][$count]['TN'] = $value['full_config']['thumb'];
//         }
//         break;

//     }
//     $count++;
//   }
}

/**
 * Get current configuration for an object based on CModels.
 *
 * @param AbstractObject $object
 *   An islandora object.
 *
 * @param array $defaults
 *   default config, as retrieved from
 *   islandora_default_thumbs_configuration().
 *
 * @return array|bool
 *   A configuration array, or FALSE if none exists.
 */
function islandora_default_thumbs_get_config_for_object($object, $defaults) {
  foreach ($object->models as $model_key => $model) {
    $default = str_replace(":", "__", $model);
    if (isset($defaults['thumb_replacements'][$model])) {
      $model_override = $defaults['thumb_replacements'][$model]['thumb_override'];
      if ($model_override[$default] != 0) {
        if (isset($object[$defaults['default_tn_dsid']]->{'content'}) &&
          !is_null($object[$defaults['default_tn_dsid']]->{'content'}) || $defaults['force']) {
          if ($defaults['force']) {
            return array(
              "thumb" => file_create_url(file_load($model_override[$default])->uri),
            );
          }
          else {
            if (in_array(md5($object[$defaults['default_tn_dsid']]->{'content'}), $defaults['compare_thumbs']) &&
              $defaults['thumb_replacements'][$model]['selected'] == 1) {
              return array(
                "thumb" => file_create_url(file_load($model_override[$default])->uri),
              );
            }
          }
        }
        else {
          // Handle the use case if a thumbnail does not exist
          // on an object. Use the CModel configured option first
          // if posible, otherwise, use the configured missing TN
          // option.
          if (isset($defaults['missing_obj_thumb'])) {
            return array(
              "thumb" => file_create_url(file_load($defaults["missing_obj_thumb"])->uri),
            );
          }
        }
        return FALSE;
      }
      if (!isset($object[$defaults['default_tn_dsid']])) {
        // Handle the use case if a thumbnail does not exist
        // on an object. Use the CModel configured option first
        // if posible, otherwise, use the configured missing TN
        // option.
        if (isset($defaults['missing_obj_thumb'])) {
          return array(
            "thumb" => file_create_url(file_load($defaults["missing_obj_thumb"])->uri),
          );
        }
      }
    }
  }
  return FALSE;
}
