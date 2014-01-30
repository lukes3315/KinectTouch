float RawDepthToMeters(int depthValue)
{
  if (depthValue < 2047)
    {
      return float(1.0 / (double(depthValue) * -0.0030711016 + 3.3309495161));
    }
  return 0.0f;
}

Vec3f DepthToWorld(int x, int y, int depthValue)
{
  static const double fx_d = 1.0 / 5.9421434211923247e+02;
  static const double fy_d = 1.0 / 5.9104053696870778e+02;
  static const double cx_d = 3.3930780975300314e+02;
  static const double cy_d = 2.4273913761751615e+02;

  Vec3f result;
  const double depth = RawDepthToMeters(depthValue);
  result.x = float((x - cx_d) * depth * fx_d);
  result.y = float((y - cy_d) * depth * fy_d);
  result.z = float(depth);
  return result;
}

Vec2i WorldToColor(const Vec3f &pt)
{
  static const Matrix4 rotationMatrix(
				      Vec3f(9.9984628826577793e-01f, 1.2635359098409581e-03f, -1.7487233004436643e-02f),
				      Vec3f(-1.4779096108364480e-03f, 9.9992385683542895e-01f, -1.2251380107679535e-02f),
				      Vec3f(1.7470421412464927e-02f, 1.2275341476520762e-02f, 9.9977202419716948e-01f));
  static const Vec3f translation(1.9985242312092553e-02f, -7.4423738761617583e-04f, -1.0916736334336222e-02f);
  static const Matrix4 finalMatrix = rotationMatrix.Transpose() * Matrix4::Translation(-translation);

  static const double fx_rgb = 5.2921508098293293e+02;
  static const double fy_rgb = 5.2556393630057437e+02;
  static const double cx_rgb = 3.2894272028759258e+02;
  static const double cy_rgb = 2.6748068171871557e+02;

  const Vec3f transformedPos = finalMatrix.TransformPoint(pt);
  const float invZ = 1.0f / transformedPos.z;

  Vec2i result;
  result.x = Utility::Bound(Math::Round((transformedPos.x * fx_rgb * invZ) + cx_rgb), 0, 639);
  result.y = Utility::Bound(Math::Round((transformedPos.y * fy_rgb * invZ) + cy_rgb), 0, 479);
  return result;
}

private int colorToDepth(Color c) {
  int colorInt = c.ToArgb();

  return (colorInt << 16) >> 16;
}
