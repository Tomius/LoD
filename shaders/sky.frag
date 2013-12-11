#version 150

const float kWorldRadius = 6371000;
const float kAtmThickness = 50000;
const vec3 kAirColor = vec3(0.32, 0.36, 0.45);
const vec3 kLightColor = vec3(1.0, 1.0, 1.0);

uniform vec4 uSunData;
vec3 uSunPos = uSunData.xyz; // The position of the sun or the Moon
float uDay = uSunData.w; // how day is it on a scale from 0 to 1 :)

uniform samplerCube uEnvMap;

// Returns the distance the light shot at look_dir travels
// in the atmosphere relative to the atmosphere's thickness
// (the more it travels there, the more reddish color it gets)
float AtmIntersection(vec3 look_dir) {
  const vec3 sun_light = vec3(0.0, -kWorldRadius, 0.0);
  const float r = kWorldRadius + kAtmThickness;
  const float sun_len_sq = dot(sun_light, sun_light);
  float look_vert = dot(look_dir, -sun_light);
  return (-look_vert + sqrt(look_vert * look_vert - sun_len_sq + r * r)) / kAtmThickness;
}

vec3 SkyColor(vec3 look_dir) {
  vec3 sun_dir = normalize(uSunPos);
  float sun_power = max(sun_dir.y + 0.12, 0.02);
  float atm_size = AtmIntersection(look_dir);
  float look_dir_sun_dist = max(dot(look_dir, sun_dir), 0.0) + 0.003 * sqrt(atm_size);
  vec4 cloud = texture(uEnvMap, look_dir);
  float cloud_border = (1.0 - cloud.a) * cloud.b;
  vec3 atm_color = max(kLightColor - kAirColor * pow(atm_size, 0.33), vec3(0.0));

  vec3 day_color, night_color;

  // Counting the day_color
  {
    // The Sun itself
    vec3 sun = atm_color *
        (look_dir_sun_dist > 0.995 + 0.004 * sun_power ? 1.0 : 0.0);

    vec3 air =
        // The sky's base color.
        min( // when this one dominates nearly the entire sky is blue, but the horizon is whitish
          kAirColor * sqrt(pow(sun_power, 0.25) * pow(atm_size, 0.75) + 0.15),
          // This one dominates at sunset / sunrise and in the night.
          // When it wins, it is a non-dominant dark grey color,
          // that lets the other effect paint the sky, but makes
          // them a bit brighter.
          vec3(sun_power) * 1.5
        ) +
        // The scattering effect near the Sun
        vec3(1.0, 0.9, 0.7) * atm_color * pow(
          min(look_dir_sun_dist + 0.001 * atm_size, 1.0),
          1024.0 / pow(atm_size, 2.0)
        ) +
        // The yellow and red tone of the sky at sunset / sunrise
        atm_color * (look_dir_sun_dist / (1.0 + pow(3.0 * sun_power, 8.0))) * pow(atm_size, 0.6) * 0.5;

    vec3 clouds =
        atm_color * (
          // The cloud's white borders. They are visible at the night too,
          // because they aren't affected by sun_power. There are 3 "separate"
          // layers of it, which are produced by 3 random functions, that have
          // like 0 geometrical meaning. Even still this effect definitely looks
          // better than having a mono-color border, and it also makes the border
          // a random shape which is kinda cool :).
          pow(min(look_dir_sun_dist * (cloud.g + cloud.b), 1.015), 64.0) * pow(cloud_border, 2.0) +
          pow(min(look_dir_sun_dist * cloud.g + cloud.b, 1.020), 32.0) * cloud_border +
          pow(min(look_dir_sun_dist * cloud.g * cloud.b, 1.010), 16.0) * pow(cloud_border, 0.5) +
          // The cloud's light grey border. It's bigger than the white border,
          // and this makes the cloud look like it has depth.
          0.7 * min(cloud.g + cloud.b * 0.5, 1.0) * sun_power +
          // The clouds main area, which is the darkest
          (cloud.g * (1.0 - cloud.b * 0.2) * 5.0) * pow(1.0 - sun_power, 2.0) * (sun_power)
        ) * 1.2 +
        // The Sun makes the clouds brighter
        kLightColor * 0.5 * min(sun_power + cloud.g * 0.4 + cloud.b * 0.1, 1.0) * sun_power;

    day_color = mix(air, clouds, cloud.a * (1.0 - 0.8 * cloud.r)) +
                vec3(1.0, 0.7, 0.5) * sun * (1.0 - 0.99 * cloud.a);

  }

  // Counting the night_color
  {
    // Just some "references" to make the code easier to read.
    // They will be optimized out anyway, so not a performance issue.
    vec3 moon_dir = sun_dir;
    float moon_power = sun_power;
    float look_dir_moon_dist = look_dir_sun_dist;

    // The Moon itself
    vec3 moon = (look_dir_moon_dist > 1.006 - 0.005 * moon_power ? vec3(0.4) : vec3(0.0));

    vec3 air =
      // The sky's base color.
      0.1 * min(
          kAirColor * sqrt(pow(moon_power, 0.25) * pow(atm_size, 0.75) + 0.15),
          vec3(moon_power) * 1.5
      ) +
      // The scattering effect near the Moon
      vec3(0.6) * pow(
          min(0.997 * look_dir_sun_dist, 1.0),
          1024.0 / pow(atm_size, 2.0)
      );

    vec3 clouds =
      // The cloud's white borders.
      atm_color * (
      pow(min(look_dir_moon_dist * (cloud.g + cloud.b), 1.015), 64.0) * pow(cloud_border, 2.0) +
      pow(min(look_dir_moon_dist * cloud.g + cloud.b, 1.020), 32.0) * cloud_border +
      pow(min(look_dir_moon_dist * cloud.g * cloud.b, 1.010), 16.0) * pow(cloud_border, 0.5) +
      // Border grey area
      0.7 * min(cloud.g + cloud.b * 0.5, 1.0) * 0.1 * moon_power +
      // The main grey area
      (cloud.g * (1.0 - cloud.b * 0.2) * 5.0) * pow(1.0 - moon_power, 2.0) * 0.1 * moon_power
      ) +
      // The Moon's effect
      kLightColor * 0.5 * min(moon_power + cloud.g * 0.4 + cloud.b * 0.1, 1.0) * 0.1 * moon_power;

    night_color = mix(air, clouds, cloud.a * (1.0 - 0.8 * cloud.r)) + moon * (1.0 - cloud.a);
  }

  return mix(night_color, day_color, uDay);
}

// Functions for other objects' lighting computations
vec3 AmbientDirection() {
  return normalize(uSunPos);
}

float isDay() {
  return uDay;
}

float SunPower() {
  return max((uDay + 0.03) * (normalize(uSunPos).y + 0.22), 0.004);
}


float AmbientPower() {
  return mix(
    0.02, // night
    0.05 * max(dot( // day
        normalize(uSunPos) + vec3(0.0, 0.12, 0.0),
        vec3(0.0, 1.0, 0.0)
    ), 0.4),
    uDay
  );
}

vec3 AmbientColor() {
  return 0.5 * SkyColor(normalize(uSunPos)) + vec3(0.5);
}
