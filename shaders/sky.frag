// Copyright (c) 2014, Tamas Csala
// This shader is based on an oglplus example:
// http://oglplus.org/oglplus/html/oglplus_2023_sky_8cpp-example.html

#version 120

const float kWorldRadius = 6371000;
const float kAtmThickness = 50000;
const vec3 kAirColor = vec3(0.32, 0.36, 0.45);
const vec3 kLightColor = vec3(1.0, 1.0, 1.0);

uniform vec3 uSunPos;
vec3 sun_pos = normalize(uSunPos);
vec3 moon_pos = -sun_pos;

uniform samplerCube uEnvMap;

float sqr(float x) {
  return x*x;
}

// Returns the distance the light shot at look_dir travels
// in the atmosphere relative to the atmosphere's thickness
// (the more it travels there, the more reddish color it gets)
float AtmIntersection(vec3 look_dir) {
  const vec3 sun_light = vec3(0.0, -kWorldRadius, 0.0);
  const float r = kWorldRadius + kAtmThickness;
  const float sun_len_sq = dot(sun_light, sun_light);
  float look_vert = dot(look_dir, -sun_light);
  return (-look_vert + sqrt(look_vert * look_vert - sun_len_sq + r*r))
         / kAtmThickness;
}

vec3 SkyColor(vec3 look_dir) {
  float atm_size = AtmIntersection(look_dir);
  vec4 cloud = textureCube(uEnvMap, look_dir);
  float cloud_border = (1.0 - cloud.a) * cloud.b;
  vec3 atm_color = max(kLightColor - kAirColor * pow(atm_size, 0.33), vec3(0.0));

  vec3 day_color, night_color;

  // Counting the day_color
  {
    float sun_power = clamp(sun_pos.y + 0.12, 0, 1);
    float look_dir_sun_dist =
          max(dot(look_dir, sun_pos), 0.0) + 0.003 * sqrt(atm_size);

    // The Sun itself
    vec3 sun = vec3(1.0, 0.7, 0.5) * atm_color *
    (look_dir_sun_dist > 0.995 + 0.004 * sun_power ? (1.0 - 0.9*cloud.a) : 0.1);

    vec3 air =
        // The sky's base color.
        min( // when this one dominates nearly the entire sky is blue, but the horizon is whitish
          kAirColor * sqrt(pow(sun_power, 0.25) * pow(atm_size, 0.75) + 0.15),
          // This one dominates at sunset / sunrise and in the night.
          // When it wins, it is a non-dominant dark grey color,
          // that lets the other effect paint the sky, but makes
          // them a bit brighter.
          vec3(sun_power) * 1.5) +
        // The scattering effect near the Sun
        vec3(1.0, 0.9, 0.7) * atm_color *
        pow(min(look_dir_sun_dist + 0.001 * atm_size, 1.0),
            1024.0 / sqr(atm_size)) +
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
          (cloud.g * (1.0 - cloud.b * 0.2) * 5.0) * pow(1.0 - sun_power, 1.7) * (sun_power)
        ) * 1.2 +
        // The Sun makes the clouds brighter
        kLightColor * 0.2 * min(sun_power + cloud.g * 0.4 + cloud.b * 0.1, 1.0) * sun_power;

    day_color = mix(air, clouds, cloud.a * (1.0 - 0.8 * cloud.r)) + sun;
  }

  // Counting the night_color
  {
    // Just some "references" to make the code easier to read.
    // They will be optimized out anyway, so not a performance issue.
    float moon_power = clamp(moon_pos.y + 0.12, 0, 1);
    float look_dir_moon_dist = max(dot(look_dir, moon_pos), 0.0) + 0.003 * sqrt(atm_size);

    // The Moon itself
    float moon = (look_dir_moon_dist > 0.9999 ? 1.0 : 0.0);

    vec3 air =
      // The sky's base color.
      0.256 * min(kAirColor * sqrt(pow(moon_power, 0.25) * pow(atm_size, 0.75) + 0.15),
                  0.75 * vec3(moon_power) * 1.5) +
      // The scattering effect near the Moon
      vec3(0.5) * pow(min(0.999 * look_dir_moon_dist, 1.0),
                          1024.0 / sqr(atm_size));

    vec3 clouds =
      // The cloud's white borders.
      atm_color * (
      pow(min(look_dir_moon_dist * (cloud.g + cloud.b), 1.015), 64.0) * pow(cloud_border, 2.0) +
      pow(min(look_dir_moon_dist * cloud.g + cloud.b, 1.020), 32.0) * cloud_border +
      pow(min(look_dir_moon_dist * cloud.g * cloud.b, 1.010), 16.0) * pow(cloud_border, 0.5) +
      // Border grey area
      0.7 * min(cloud.g + cloud.b * 0.5, 1.0) * 0.1 * moon_power +
      // The main grey area
      (cloud.g * (1.0 - cloud.b * 0.2) * 5.0) * pow(1.0 - moon_power, 2.0) * 0.1 * moon_power) +
      // The Moon's effect
      kLightColor * 0.5 * min(moon_power + cloud.g * 0.4 + cloud.b * 0.1, 1.0) * 0.1 * moon_power;

    night_color = mix(air, clouds, cloud.a * (1.0 - 0.8 * cloud.r))
                  + vec3(0.4) * moon * (1.0 - cloud.a);
  }

  vec3 final_color = clamp(night_color + day_color, 0, 1);
  return pow(final_color, vec3(2.2)); // srgb -> linear
}

// Functions for other objects' lighting computations
vec3 SunPos() { return sun_pos; }
vec3 MoonPos() { return moon_pos; }

float SunPower() { return clamp(sun_pos.y, 0, 1); }
float MoonPower() { return clamp(moon_pos.y, 0, 1); }

vec3 SunColor() { return vec3(1.0, 0.9, 0.75); }
vec3 MoonColor() { return vec3(0.4); }

float AmbientPower() { return 0.15 * max(max(SunPower(), MoonPower()), 0.2); }
vec3 AmbientColor() {
  return max(SunPower() * SunColor() + MoonPower() * MoonColor(),
             vec3(AmbientPower()));
}
