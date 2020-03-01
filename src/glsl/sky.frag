// Copyright (c) 2014, Tamas Csala
// This shader is based on an oglplus example:
// http://oglplus.org/oglplus/html/oglplus_2023_sky_8cpp-example.html

#version 430

#export vec3 SkyColor(vec3 look_dir);
#export vec3 SunPos();
#export vec3 MoonPos();
#export float SunPower();
#export float MoonPower();
#export float AmbientPower();
#export vec3 SunColor();
#export vec3 MoonColor();
#export vec3 AmbientColor();

const float kWorldRadius = 6371000;
const float kAtmThickness = 50000;
const vec3 kAirColor = vec3(0.32, 0.36, 0.45);
const vec3 kLightColor = vec3(1.0, 1.0, 1.0);

uniform vec3 uSunPos;
vec3 sun_pos = normalize(uSunPos);
vec3 moon_pos = -sun_pos;

float sky_sqr(float x) {
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
  vec3 atm_color = max(kLightColor - kAirColor * pow(atm_size, 0.33), vec3(0.0));

  vec3 day_color, night_color;

  // Counting the day_color
  {
    float sun_power = clamp(sun_pos.y + 0.12, 0.1, 1.0);
    float look_dir_sun_dist =
          max(dot(look_dir, sun_pos), 0.0) + 0.003 * sqrt(atm_size);

    // The Sun itself
    vec3 sun = vec3(1.0, 0.7, 0.5) * atm_color *
    (look_dir_sun_dist > 0.995 + 0.004 * sun_power ? 0.9 : 0.1);

    vec3 air =
        // The sky's base color.
        min( // when this one dominates nearly the entire sky is blue, but the horizon is whitish
          kAirColor * sqrt(pow(sun_power, 0.25) * pow(atm_size, 0.75) + 0.15),
          // This one dominates at sunset / sunrise and in the night.
          // When it wins, it is a non-dominant dark grey color,
          // that lets the other effect paint the sky, but makes
          // them a bit brighter.
          (3+look_dir.y)/3 *  vec3(sun_power)) +
        // The scattering effect near the Sun
        vec3(1.0, 0.9, 0.7) * atm_color *
        pow(min(look_dir_sun_dist + 0.001 * atm_size, 1.0),
            1024.0 / sky_sqr(atm_size)) +
        // The yellow and red tone of the sky at sunset / sunrise
        atm_color * (look_dir_sun_dist / (0.7 + pow(2.9 * sun_power, 4.0))) * pow(atm_size, 0.6) * 0.5;

    day_color = air + sun;
  }

  // Counting the night_color
  {
    float moon_power = 0.25 * clamp(moon_pos.y + 0.12, 0.1, 1.0);
    float look_dir_moon_dist = max(dot(look_dir, moon_pos), 0.0)
                              + 0.0005 * sqrt(atm_size);

    // The Moon itself
    float moon = (look_dir_moon_dist > 0.9999 ? 1.0 : 0.0);

    vec3 air =
      // The sky's base color.
      0.256 * min(kAirColor * sqrt(pow(moon_power, 0.25) * pow(atm_size, 0.75) + 0.05),
                  vec3(moon_power)) +
      // The scattering effect near the Moon
      vec3(0.2) * pow(min(look_dir_moon_dist + 0.001 * atm_size, 1.0),
                          1024.0 / sky_sqr(atm_size));

    night_color = air + vec3(0.4) * moon;
  }

  vec3 final_color = clamp(night_color + day_color, 0.0, 1.0);
  return final_color*final_color; // srgb -> linear
}

// Functions for other objects' lighting computations
vec3 SunPos() { return sun_pos; }
vec3 MoonPos() { return moon_pos; }

float SunPower() { return clamp(sun_pos.y, 0.0, 1.0); }
float MoonPower() { return clamp(moon_pos.y, 0.0, 1.0); }

vec3 SunColor() { return vec3(1.0, 0.9, 0.75); }
vec3 MoonColor() { return vec3(0.4); }

float AmbientPower() { return 0.25 * max(max(SunPower(), MoonPower()), 0.3); }
vec3 AmbientColor() {
  return SunPower() * SunColor() + MoonPower()*MoonColor()/2;
}
