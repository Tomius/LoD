// Copyright (c) 2014, Tamas Csala

#include <ctime>
#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include "../../oglwrap/debug/insertion.h"
#include "../transform.h"

using Transform = engine::Transformation<double>;

constexpr double epsilon = 1e-4;
size_t fail_num = 0;

template<typename T>
void AssertEquals(T a, T b, const std::string& msg) {
  if (a != b) {
    std::cout << "Failed: " + msg << std::endl;
    std::cout << a << " != " << b << std::endl;
    fail_num++;
  }
}

bool CheckDouble(double a, double b) {
  return fabs(a - b) > epsilon;
}

void AssertEquals(double a, double b, const std::string& msg) {
  if (CheckDouble(a, b)) {
    std::cout << "Failed: " + msg << std::endl;
    std::cout << a << " != " << b << std::endl;
    fail_num++;
  }
}

void AssertEquals(const glm::dvec3& a, const glm::dvec3& b, const std::string& msg) {
  bool failure = false;
  failure |= CheckDouble(a.x, b.x);
  failure |= CheckDouble(a.y, b.y);
  failure |= CheckDouble(a.z, b.z);
  if (failure) {
    std::cout << "Failed: " + msg << std::endl;
    std::cout << a << " != " << b << std::endl;
    fail_num++;
  }
}

void AssertEquals(const glm::dquat& a, const glm::dquat& b, const std::string& msg) {
  bool failure = false;
  failure |= CheckDouble(a.x, b.x);
  failure |= CheckDouble(a.y, b.y);
  failure |= CheckDouble(a.z, b.z);
  failure |= CheckDouble(a.w, b.w);
  if (failure) {
    std::cout << "Failed: " + msg << std::endl;
    std::cout << a << " != " << b << std::endl;
    fail_num++;
  }
}

void AssertEquals(const Transform& a, const Transform& b, const std::string& msg) {
  AssertEquals(a.getParent(), b.getParent(), msg);
  AssertEquals(a.local_pos(), b.local_pos(), msg);
  AssertEquals(a.local_rot(), b.local_rot(), msg);
  AssertEquals(a.local_scale(), b.local_scale(), msg);
}

glm::dvec3 RandomVec() {
  return glm::dvec3(
    100.0 * rand() / RAND_MAX,
    100.0 * rand() / RAND_MAX,
    100.0 * rand() / RAND_MAX
  );
}

glm::dquat RandomQuat() {
  return glm::dquat(
    100.0 * rand() / RAND_MAX,
    rand() / RAND_MAX,
    rand() / RAND_MAX,
    rand() / RAND_MAX
  );
}

void TestParentChild(Transform& parent,
                     Transform& child,
                     Transform& grand_child) {
  AssertEquals(&parent, child.getParent(), "Setting up parent relation");
  AssertEquals(&child, parent.getChildren()[0], "Setting up child relation");
  AssertEquals(parent.pos(), child.pos(), "Location inheriting");
  AssertEquals(child.pos(), grand_child.pos(), "Two levels Location inheriting");
}


void CheckPos(Transform& parent, Transform& child, const std::string& msg) {
  AssertEquals(
    child.pos(), parent.pos() + parent.rotateAndScale() * child.local_pos(), msg
  );
}

void TestLocRotScaleLevel1(Transform& parent, Transform& child) {
  Transform p = parent;

  // Location
  child.set_local_pos(RandomVec());

  CheckPos(parent, child, "Translation effect on child");
  AssertEquals(parent, p, "Child's translation has no effect parent");

  // Rotation
  parent.set_rot(RandomQuat());

  CheckPos(parent, child, "Rotation effect on child");
  AssertEquals(parent.pos(), p.pos(), "Rotation invariant on local_pos");

  p = parent;
  child.set_rot(parent.rot());
  AssertEquals(parent, p, "Child's rotation has no effect parent");

  // Scale
  parent.set_local_scale(RandomVec());

  CheckPos(parent, child, "Scale effect on child's position");
  AssertEquals(parent.pos(), p.pos(), "Scale invariant on local_pos");

  p = parent;
  child.set_scale(parent.scale());
  AssertEquals(parent, p, "Child's scaling has no effect parent");
}

void TestLocRotScaleLevel2(Transform& gparent, Transform& parent, Transform& child) {
  Transform p = parent;
  Transform gp = gparent;

  // Location
  child.set_local_pos(RandomVec());

  CheckPos(parent, child, "Translation effect on child");
  AssertEquals(parent, p, "Child's translation has no effect parent");

  // Rotation
  parent.set_rot(RandomQuat());

  CheckPos(parent, child, "Rotation effect on child");
  AssertEquals(parent.pos(), p.pos(), "Rotation invariant on local_pos");

  p = parent;
  gp = gparent;
  child.set_rot(parent.rot());
  AssertEquals(parent, p, "Child's rotation has no effect on parent");
  AssertEquals(gparent, gp, "Child's rotation has no effect on gparent");

  // Scale
  parent.set_local_scale(RandomVec());

  CheckPos(parent, child, "Scale effect on child");
  AssertEquals(parent.pos(), p.pos(), "Scale invariant on local_pos");

  p = parent;
  gp = gparent;
  child.set_scale(parent.scale());
  AssertEquals(parent, p, "Child's scaling has no effect parent");
  AssertEquals(gparent, gp, "Child's scaling has no effect on gparent");
}

void DirectionTest() {
  Transform t;

  AssertEquals(t.forward(), glm::dvec3(0, 0, -1), "Forward");
  AssertEquals(t.up(), glm::dvec3(0, 1, 0), "Up");
  AssertEquals(t.right(), glm::dvec3(1, 0, 0), "Right");

  t.set_rot(glm::quat_cast(glm::rotate(glm::dmat4(), M_PI_2, glm::dvec3(1, 0, 0))));

  AssertEquals(t.forward(), glm::dvec3(0, 1, 0), "Forward");
  AssertEquals(t.up(), glm::dvec3(0, 0, 1), "Up");
  AssertEquals(t.right(), glm::dvec3(1, 0, 0), "Right");

  for (int i = 0; i < 1000; ++i) {
    t.set_rot(RandomQuat());
    AssertEquals(glm::dot(t.forward(), t.right()), 0.0, "Forward and right should be ortho");
    AssertEquals(glm::dot(t.forward(), t.up()), 0.0, "Forward and up should be ortho");
    AssertEquals(glm::dot(t.right(), t.up()), 0.0, "Right and up should be ortho");
  }
}


int GetParentsNum(Transform* t) {
  Transform* parent = t->getParent();
  if (parent) {
    return GetParentsNum(parent) + 1;
  } else {
    return 0;
  }
}
void GlobalSettings(Transform& t) {
  glm::dvec3 v{RandomVec()}, u{RandomVec()};
  glm::dquat q{RandomQuat()};

  int pnum = GetParentsNum(&t);
  std::string prnts = " on a transform with " + std::to_string(pnum) + " parents";

  t.set_pos(v);
  AssertEquals(t.pos(), v, "Setting global position" + prnts);

  t.set_rot(q);
  AssertEquals(t.rot(), q, "Setting global rotation" + prnts);

  t.set_scale(v);
  AssertEquals(t.scale(), v, "Setting global scaling" + prnts);

  v = RandomVec();
  t.set_forward(v);
  AssertEquals(t.forward(), glm::normalize(v), "Setting global forward" + prnts);

  v = RandomVec();
  t.set_right(v);
  AssertEquals(t.right(), glm::normalize(v), "Setting global right" + prnts);

  v = RandomVec();
  t.set_up(v);
  AssertEquals(t.up(), glm::normalize(v), "Setting global up" + prnts);

  for (int i = 0; i < 1000; ++i) {
    t.set_rot(v = glm::normalize(RandomVec()), u = glm::normalize(RandomVec()));
    AssertEquals(t.rot()*v, u, "Setting rot with 'v', 'u'" + prnts);
  }

  // There are special cases for rot
  t.set_rot(v, v);
  AssertEquals(t.rot()*v, v, "Setting rot with 'v', 'v'" + prnts);

  t.set_rot(v, -v);
  AssertEquals(t.rot()*v, -v, "Setting rot with 'v', '-v'" + prnts);
}

int main() {
  srand(time(nullptr));

  Transform parent, child, grand_child;
  parent.set_pos(RandomVec());
  parent.addChild(child);
  child.addChild(grand_child);
  TestParentChild(parent, child, grand_child);

  // Test with a thousand random transformations
  for (int i = 0; i < 1000; ++i) {
    TestLocRotScaleLevel1(parent, child);
    TestLocRotScaleLevel2(parent, child, grand_child);
  }

  DirectionTest();
  GlobalSettings(parent);
  GlobalSettings(child);
  GlobalSettings(grand_child);

  if (fail_num) {
    std::cout << "Number of failures: " << fail_num << std::endl;
  } else {
    std::cout << "Test was successful" << std::endl;
  }
}
