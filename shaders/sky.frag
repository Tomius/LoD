#version 330

const float WorldRadius = 6371000;
const float AtmThickness = 50000;
const vec3 AirColor = vec3(0.32, 0.36, 0.45);
const vec3 LightColor = vec3(1.0, 1.0, 1.0);

uniform vec4 SunData;
vec3 SunPosition = SunData.xyz; // The position of the sun or the Moon
float Day = SunData.w; // how day is it on a scale from 0 to 1 :)

uniform samplerCube EnvMap;

// Returns the distance the light shot at lookDir travels
// in the atmosphere relative to the atmosphere's thickness
// (the more it travels there, the more reddish color it gets)
float atm_intersection(vec3 lookDir) {
    const vec3 sunLight = vec3(0.0, -WorldRadius, 0.0);
    const float r = WorldRadius + AtmThickness;
    const float sunLenSq = dot(sunLight, sunLight);
    float lookVert = dot(lookDir, -sunLight);
    return (-lookVert + sqrt(lookVert * lookVert - sunLenSq + r * r)) / AtmThickness;
}

vec3 sky_color(vec3 lookDir) {

    vec3 sunDir = normalize(SunPosition);
    float sunPower = max(sunDir.y + 0.12, 0.02);
    float atmSize = atm_intersection(lookDir);
    float lookDir_Sun_dist = max(dot(lookDir, sunDir), 0.0) + 0.003 * sqrt(atmSize);
    vec4 cloud = texture(EnvMap, lookDir);
    float cloudBorder = (1.0 - cloud.a) * cloud.b;
    vec3 atmColor = max(LightColor - AirColor * pow(atmSize, 0.33), vec3(0.0));

    vec3 dayColor, nightColor;

    // Counting the dayColor
    {
        vec3 atmColor = max(LightColor - AirColor * pow(atmSize, 0.33), vec3(0.0));

        // The Sun itself
        vec3 Sun = atmColor *
            (lookDir_Sun_dist > 0.995 + 0.004 * sunPower ? 1.0 : 0.0);

        vec3 Air =
            // The sky's base color.
            min( // when this one dominates nearly the entire sky is blue, but the horizon is whitish
                AirColor * sqrt(pow(sunPower, 0.25) * pow(atmSize, 0.75) + 0.15),
                // This one dominates at sunset / sunrise and in the night.
                // When it wins, it is a non-dominant dark grey color,
                // that lets the other effect paint the sky, but makes
                // them a bit brighter.
                vec3(sunPower) * 1.5
            ) +
            // The scattering effect near the Sun
            vec3(1.0, 0.9, 0.7) * atmColor * pow(
                min(lookDir_Sun_dist + 0.001 * atmSize, 1.0),
                1024.0 / pow(atmSize, 2.0)
            ) +
            // The yellow and red tone of the sky at sunset / sunrise
            atmColor * (lookDir_Sun_dist / (1.0 + pow(3.0 * sunPower, 8.0))) * pow(atmSize, 0.6) * 0.5;

        vec3 Clouds =
            atmColor * (
                // The cloud's white borders. They are visible at the night too,
                // because they aren't affected by sunPower. There are 3 "separate"
                // layers of it, which are produced by 3 random functions, that have
                // like 0 geometrical meaning. Even still this effect definitely looks
                // better than having a mono-color border, and it also makes the border
                // a random shape which is kinda cool :).
                pow(min(lookDir_Sun_dist * (cloud.g + cloud.b), 1.015), 64.0) * pow(cloudBorder, 2.0) +
                pow(min(lookDir_Sun_dist * cloud.g + cloud.b, 1.020), 32.0) * cloudBorder +
                pow(min(lookDir_Sun_dist * cloud.g * cloud.b, 1.010), 16.0) * pow(cloudBorder, 0.5) +
                // The cloud's light grey border. It's bigger than the white border,
                // and this makes the cloud look like it has depth.
                0.7 * min(cloud.g + cloud.b * 0.5, 1.0) * sunPower +
                // The clouds main area, which is the darkest
                (cloud.g * (1.0 - cloud.b * 0.2) * 5.0) * pow(1.0 - sunPower, 2.0) * (sunPower)
            ) * 1.2 +
            // The Sun makes the clouds brighter
            LightColor * 0.5 * min(sunPower + cloud.g * 0.4 + cloud.b * 0.1, 1.0) * sunPower;

        dayColor = mix(Air,
                       Clouds,
                       cloud.a * (1.0 - 0.8 * cloud.r)
                   ) + vec3(1.0, 0.7, 0.5) * Sun * (1.0 - 0.99 * cloud.a);

    }

    // Counting the nightColor
    {
        // Just some "references" to make the code easier to read.
        // They will be optimized out anyway, so not a performance issue.
        vec3 moonDir = sunDir;
        float moonPower = sunPower;
        float lookDir_Moon_dist = lookDir_Sun_dist;

        // The Moon itself
        vec3 Moon = (lookDir_Moon_dist > 0.999 + 0.001 * moonPower ? vec3(0.4) : vec3(0.0));

        vec3 Air =
            // The sky's base color.
            0.1 * min(
                AirColor * sqrt(pow(moonPower, 0.25) * pow(atmSize, 0.75) + 0.15),
                vec3(moonPower) * 1.5
            ) +
            // The scattering effect near the Moon
            vec3(0.6) * pow(
                min(0.998 * lookDir_Sun_dist + 0.002 * moonPower, 1.0),
                1024.0 / pow(atmSize, 2.0)
            );

        vec3 Clouds =
                // The cloud's white borders.
                atmColor * (
                pow(min(lookDir_Moon_dist * (cloud.g + cloud.b), 1.015), 64.0) * pow(cloudBorder, 2.0) +
                pow(min(lookDir_Moon_dist * cloud.g + cloud.b, 1.020), 32.0) * cloudBorder +
                pow(min(lookDir_Moon_dist * cloud.g * cloud.b, 1.010), 16.0) * pow(cloudBorder, 0.5) +
                // Border grey area
                0.7 * min(cloud.g + cloud.b * 0.5, 1.0) * 0.1 * moonPower +
                // The main grey area
                (cloud.g * (1.0 - cloud.b * 0.2) * 5.0) * pow(1.0 - moonPower, 2.0) * 0.1 * moonPower
                ) +
                // The moon's effect
                LightColor * 0.5 * min(moonPower + cloud.g * 0.4 + cloud.b * 0.1, 1.0) * 0.1 * moonPower;

        nightColor = mix(Air,
                        Clouds,
                        cloud.a * (1.0 - 0.8 * cloud.r)
                    ) + Moon * (1.0 - cloud.a);
    }

    return mix(nightColor, dayColor, Day);
}

// Functions for other objects' lighting computations
vec3 AmbientDirection(){
    return SunPosition;
}

float SunPower(){
    return
        max((Day + 0.03) * (normalize(SunPosition).y + 0.12), 0.002);
}

float AmbientPower(){
    return
        mix(
            0.2 * 0.02, // night
            0.2 * max(dot( // day
                normalize(SunPosition) + vec3(0.0, 0.12, 0.0),
                vec3(0.0, 1.0, 0.0)
            ), 0.02),
            Day
        );
}

// FIXME
vec3 AmbientColor(){
    return AirColor;
    //return sky_color(normalize(SunPosition));
}
